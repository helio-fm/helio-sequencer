/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "ScriptingPlayground.h"
#include "ShadowUpwards.h"
#include "ShadowLeftwards.h"
#include "ShadowRightwards.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "MainLayout.h"
#include "ProjectNode.h"
#include "ProjectMetadata.h"
#include "ProjectTimeline.h"
#include "KeySignaturesSequence.h"
#include "TimeSignaturesSequence.h"
#include "AnnotationsSequence.h"
#include "PianoSequence.h"
#include "SequencerOperations.h"
#include "ScriptEngine.h"
#include "ScriptTokeniser.h"
#include "IconButton.h"
#include "HotkeyScheme.h"
#include "HelioTheme.h"
#include "Config.h"
#include "SerializationKeys.h"
#include "ComponentIDs.h"
#include "CommandIDs.h"
#include "ColourIDs.h"

//===----------------------------------------------------------------------===//
// Custom CodeEditorComponent
//===----------------------------------------------------------------------===//

#pragma region ScriptingPlaygroundEditor

class ScriptingPlaygroundErrorMark final : public Component
{
public:

    ScriptingPlaygroundErrorMark() noexcept
    {
        this->setOpaque(true);
        this->setAccessible(false);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->fillColour);
        g.fillRect(this->getLocalBounds());
    }

private:

    const Colour fillColour = findDefaultColour(ColourIDs::CodeEditor::error);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScriptingPlaygroundErrorMark)
};

class ScriptingPlaygroundPopup final : public Component
{
public:

    ScriptingPlaygroundPopup() noexcept
    {
        this->setOpaque(false);
        this->setAccessible(false);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);

        const auto editorConfig = App::Config().getUiFlags()->getScriptEditorSettings();
        Font font(editorConfig.fontSize);
        font.setTypefaceName(editorConfig.fontName);

        this->label = make<Label>();
        this->addAndMakeVisible(this->label.get());
        this->label->setFont(font);
        this->label->setJustificationType(Justification::topLeft);
        this->label->setBorderSize({ 7, 6, 7, 6 });
        this->label->setMinimumHorizontalScale(0.995f);
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->fillColour);
        const auto labelBounds = this->getLabelBounds();
        g.fillRect(labelBounds.reduced(0, 1));
        g.fillRect(labelBounds.reduced(1, 0));

        g.setColour(this->frameColour);
        HelioTheme::drawDashedHorizontalLine(g,
            1.f, 0.f, jmax(0.f, this->underlineWidth - 2.f));

        //HelioTheme::drawBrackets(g, labelBounds, 7, 3, 1);
    }

    void show(const String &text, const Rectangle<int> &pointAtBounds)
    {
        jassert(this->getParentComponent() != nullptr);

        this->underlineWidth = float(pointAtBounds.getWidth());

        const auto textWidth = this->label->getFont().getStringWidth(text);
        const auto width = textWidth + this->label->getBorderSize().getLeftAndRight();
        const auto position = pointAtBounds.getBottomLeft().translated(0, -1);
        const auto margin = 3;
        this->setBounds(Rectangle<int>(position.x, position.y,
            jmax(width, pointAtBounds.getWidth()), labelHeight + margin));

        this->label->setBounds(this->getLabelBounds());
        this->label->setText(text, dontSendNotification);

        this->setVisible(true);
    }

    void hide()
    {
        this->setVisible(false);
    }

private:

    float underlineWidth = 0.f;

    UniquePointer<Label> label;
    static constexpr int labelHeight = 26;
    Rectangle<int> getLabelBounds() const
    {
        return this->getLocalBounds().removeFromBottom(labelHeight);
    }

    const Colour frameColour =
        findDefaultColour(CaretComponent::caretColourId)
            .withMultipliedAlpha(0.420f);

    const Colour fillColour =
        findDefaultColour(ColourIDs::CodeEditor::popup);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScriptingPlaygroundPopup)
};

ScriptingPlaygroundEditor::ScriptingPlaygroundEditor(CodeDocument &document,
    UniquePointer<ScriptTokeniser> codeTokeniser) noexcept :
    CodeEditorComponent(document, codeTokeniser.get()),
    tokeniser(move(codeTokeniser))
{
    this->setScrollbarThickness(2);
    this->horizontalScrollBar.setVisible(false);
    this->verticalScrollBar.setColour(ScrollBar::thumbColourId,
        findDefaultColour(CodeEditorComponent::highlightColourId));

    this->popup = make<ScriptingPlaygroundPopup>();
    this->addChildComponent(this->popup.get());

    this->errorMark = make<ScriptingPlaygroundErrorMark>();
    this->addChildComponent(this->errorMark.get());

    this->getDocument().addListener(this);
}

void ScriptingPlaygroundEditor::configure(int defaultCaretPosition,
    int defaultStartLine, int tabSize) noexcept
{
    this->setTabSize(tabSize, false);
    this->scrollToLine(defaultStartLine);
    this->moveCaretTo(CodeDocument::Position(this->getDocument(), defaultCaretPosition), false);
}

ScriptingPlaygroundEditor::~ScriptingPlaygroundEditor()
{
    this->getDocument().removeListener(this);
}

const String &ScriptingPlaygroundEditor::getHighlightedToken() const noexcept
{
    return this->highlightedToken;
}

Optional<ScriptEngine::Breakpoint> ScriptingPlaygroundEditor::getBreakpoint() const
{
    if (!this->breakpoint.hasValue())
    {
        return this->breakpoint;
    }

    ScriptEngine::Breakpoint result = *this->breakpoint;
    result.parentListRange = {};
    int minRange = INT_MAX;
    // finds parent list range instead of symbol's range:
    for (const auto &range : this->bracketRanges)
    {
        if (range.contains(this->breakpoint->parentListRange.getStart()) &&
            range.contains(this->breakpoint->parentListRange.getEnd()))
        {
            if (range.getLength() >= minRange)
            {
                break;
            }

            minRange = range.getLength();
            result.parentListRange = range;
        }
    }

    return result;
}

void ScriptingPlaygroundEditor::setBreakpointInfo(const String &info)
{
    if (!this->breakpointTokenBounds.isEmpty())
    {
        this->popup->show(info, this->breakpointTokenBounds);
    }
}

void ScriptingPlaygroundEditor::setError(const Range<int> &range, const String &text)
{
    this->errorRange = range;

    this->tokeniser->setErrorRange(range);
    this->retokenise();

    if (range.isEmpty())
    {
        this->errorMark->setVisible(false);
    }
    else
    {
        this->updateErrorRangeBounds();
        this->errorMark->setVisible(true);
    }
}

void ScriptingPlaygroundEditor::updateParsingData(const Array<Range<int>> &ranges)
{
    this->bracketRanges = ranges;
    this->tokeniser->updateParsingData(ranges);
    this->retokenise();
}

void ScriptingPlaygroundEditor::updateEvaluationData(const StringArray &functionNames)
{
    this->tokeniser->updateEvaluationData(functionNames);
    this->retokenise();
}

void ScriptingPlaygroundEditor::retokenise()
{
    this->tokeniser->setCaretPosition(this->getCaretPosition());
    this->tokeniser->setHighlightedToken(this->getHighlightedToken());
    CodeEditorComponent::retokenise(0, 0);
}

void ScriptingPlaygroundEditor::updateErrorRangeBounds()
{
    if (this->errorRange.isEmpty())
    {
        return;
    }

    const CodeDocument::Position start(this->getDocument(), this->errorRange.getStart());
    const CodeDocument::Position end(this->getDocument(), this->errorRange.getEnd());
    const auto bounds = this->getCharacterBounds(start).
        getUnion(this->getCharacterBounds(end)).
        withWidth(3).withX(0);
    this->errorMark->setBounds(bounds);
}

// more conventional multiple-click-and-drag behaviour
void ScriptingPlaygroundEditor::dragSelection(CodeDocument::Position position, bool fullLines)
{
    auto tokenStart = position;
    auto tokenEnd = position;

    if (fullLines)
    {
        this->getDocument().findLineContaining(position, tokenStart, tokenEnd);
    }
    else
    {
        this->getDocument().findTokenContaining(position, tokenStart, tokenEnd);
    }

    if (this->selectionAnchorStart.getPosition() < tokenStart.getPosition())
    {
        tokenStart = this->selectionAnchorStart;
    }
    else if (this->selectionAnchorEnd.getPosition() > tokenEnd.getPosition())
    {
        tokenEnd = this->selectionAnchorEnd;
    }

    // a few more hacks to avoid glitches with selectRegion and mouse wheel
    this->setVisible(false);
    this->selectRegion(tokenStart, tokenEnd);
    this->setVisible(true);
    this->grabKeyboardFocus();
    this->updateCaretPosition();
}

void ScriptingPlaygroundEditor::mouseDown(const MouseEvent &e)
{
    this->resetBreakpointPopup();

    const auto timeSinceLastMouseDown =
        Time::getCurrentTime() - this->lastMouseDownTime;

    const auto currentPosition = this->getPositionAt(e.x, e.y);

    if ((timeSinceLastMouseDown.inMilliseconds() > MouseEvent::getDoubleClickTimeout()) ||
        currentPosition != this->lastMouseDownPosition)
    {
        this->numFastClicks = 1;
    }
    else
    {
        this->numFastClicks++;

        if (this->numFastClicks == 2) // selects words
        {
            this->getDocument().findTokenContaining(currentPosition,
                this->selectionAnchorStart, this->selectionAnchorEnd);
        }
        else // selects lines
        {
            this->getDocument().findLineContaining(currentPosition,
                this->selectionAnchorStart, this->selectionAnchorEnd);
        }
    }

    //DBG(this->numFastClicks);
    if (this->numFastClicks < 2) // selects letters
    {
        CodeEditorComponent::mouseDown(e);
    }
    else if (this->numFastClicks == 2) // selects words
    {
        this->dragSelection(currentPosition, false);
    }
    else // selects lines
    {
        this->dragSelection(currentPosition, true);
    }

    this->lastMouseDownTime = Time::getCurrentTime();
    this->lastMouseDownPosition = currentPosition;
    this->lastMouseWheelCounter =
        Desktop::getInstance().getMouseWheelMoveCounter();
}

void ScriptingPlaygroundEditor::mouseDrag(const MouseEvent &e)
{
    // a workaround for how JUCE updates scrollbars
    // (without this, wheel won't work while dragging)
    const auto wheelCounter =
        Desktop::getInstance().getMouseWheelMoveCounter();
    if (this->lastMouseWheelCounter != wheelCounter)
    {
        this->lastMouseWheelCounter = wheelCounter;
        e.source.triggerFakeMove(); // schedule later update
        return;
    }

    if (this->numFastClicks < 2) // selects letters
    {
        CodeEditorComponent::mouseDrag(e);
    }
    else if (this->numFastClicks == 2) // selects words
    {
        this->dragSelection(this->getPositionAt(e.x, e.y), false);
    }
    else // selects lines
    {
        this->dragSelection(this->getPositionAt(e.x, e.y), true);
    }
}

void ScriptingPlaygroundEditor::mouseUp(const MouseEvent &e)
{
    CodeEditorComponent::mouseUp(e);

    this->resetBreakpointPopup();

    const auto selectionStart = this->getSelectionStart();
    const auto selectionEnd = this->getSelectionEnd();
    if (selectionStart != selectionEnd &&
        (selectionEnd.getPosition() - selectionStart.getPosition() < ScriptTokeniser::maxTokenLength) &&
        selectionStart.getLineNumber() == selectionEnd.getLineNumber())
    {
        this->highlightedToken = this->getDocument().getTextBetween(selectionStart, selectionEnd);
        this->retokenise();
    }
    else if (this->highlightedToken.isNotEmpty())
    {
        this->highlightedToken.clear();
        this->retokenise();
    }
}

void ScriptingPlaygroundEditor::mouseDoubleClick(const MouseEvent &e)
{
    dragType = notDragging;
}

// displaying symbol info on mouse hover
void ScriptingPlaygroundEditor::mouseMove(const MouseEvent &e)
{
    if (!this->breakpointTokenBounds.isEmpty() &&
        !this->breakpointTokenBounds.contains(e.getPosition()))
    {
        this->resetBreakpointPopup();
    }
    else
    {
        this->lastMouseMovePosition = e.getPosition();
        this->startTimer(250);
    }
}

void ScriptingPlaygroundEditor::mouseWheelMove(const MouseEvent &event,
    const MouseWheelDetails &wheel)
{
    this->resetBreakpointPopup();

    CodeEditorComponent::mouseWheelMove(event, wheel);
}

static void findNonWhitespaceToken(const CodeDocument::Position &pos,
    CodeDocument::Position &outStart, CodeDocument::Position &outEnd) noexcept
{
    outEnd = pos;
    while (script::isSymbolBody(outEnd.getCharacter()))
    {
        outEnd.moveBy(1);
    }

    outStart = outEnd;
    while (outStart.getIndexInLine() > 0 &&
        script::isSymbolBody(outStart.movedBy(-1).getCharacter()))
    {
        outStart.moveBy(-1);
    }
}

void ScriptingPlaygroundEditor::timerCallback()
{
    this->stopTimer();

    const auto positionUnderMouse =
        this->getPositionAt(this->lastMouseMovePosition.x,
            this->lastMouseMovePosition.y);

    auto tokenStart = positionUnderMouse;
    auto tokenEnd = positionUnderMouse;
    findNonWhitespaceToken(positionUnderMouse, tokenStart, tokenEnd);

    const auto tokenRange = Range<int>(tokenStart.getPosition(), tokenEnd.getPosition());
    const auto tokenBounds = this->getTextBounds(tokenRange).getBounds();
    if (!tokenBounds.contains(this->lastMouseMovePosition))
    {
        this->resetBreakpointPopup();
        return; // mouse position is far off to the right
    }

    if (tokenStart == tokenEnd ||
        tokenBounds != this->breakpointTokenBounds)
    {
        this->popup->hide();
    }

    if (tokenStart != tokenEnd &&
        tokenBounds != this->breakpointTokenBounds)
    {
        this->breakpointTokenBounds = tokenBounds;

        this->breakpoint = {
            this->getDocument().getTextBetween(tokenStart, tokenEnd),
            Range<int>(tokenStart.getPosition(), tokenEnd.getPosition()) };

        if (auto *parent = this->getParentComponent())
        {
            parent->postCommandMessage(CommandIDs::ScriptEditorReevaluate);
        }
    }
}

// better indentation for return keys
static int findFirstNonWhitespaceChar(StringRef line) noexcept
{
    int i = 0;
    auto t = line.text;
    while (!t.isEmpty())
    {
        if (!t.isWhitespace())
        {
            return i;
        }
        ++t;
        ++i;
    }

    return 0;
}

void ScriptingPlaygroundEditor::handleReturnKey()
{
    const int myIndentLevel =
        findFirstNonWhitespaceChar(this->getCaretPos().getLineText());

    this->newTransaction();
    this->insertTextAtCaret(this->getDocument().getNewLineCharacters());
    if (this->getCaretPos().getCharacter() != ' ')
    {
        const auto numTabs = myIndentLevel / this->getTabSize();
        for (int i = 0; i < numTabs; ++i)
        {
            this->insertTabAtCaret();
        }
    }
}

bool ScriptingPlaygroundEditor::keyPressed(const KeyPress &key)
{
    // pass the hotkey keypresses up
    for (const auto &usedAsHotkey :
        ScriptingPlayground::getAllScriptingPlaygroundHotkeys())
    {
        if (key == usedAsHotkey)
        {
            return false;
        }
    }

    // more conventional tab / shift-tab behaviour
    if (key == KeyPress::tabKey || key.getTextCharacter() == '\t')
    {
        if (key.getModifiers().isShiftDown())
        {
            this->unindentSelection();
        }
        else
        {
            if (this->selectionStart.getLineNumber() !=
                this->selectionEnd.getLineNumber())
            {
                this->indentSelection();
            }
            else
            {
                this->handleTabKey();
            }
        }

        return true;
    }

    // more conventional shift-delete behaviour
    if (this->getHighlightedRegion().isEmpty() &&
        key == KeyPress(KeyPress::deleteKey, ModifierKeys::shiftModifier, 0))
    {
        this->newTransaction();
        this->moveCaretToStartOfLine(false);
        this->moveCaretTo(CodeDocument::Position(this->getDocument(),
            this->getCaretPos().getLineNumber(), 0), false);
        this->moveCaretToEndOfLine(true);
        this->moveCaretRight(false, true);
        this->cutToClipboard();
        return true;
    }

    return CodeEditorComponent::keyPressed(key);
}

void ScriptingPlaygroundEditor::selectNext()
{
    if (this->getHighlightedRegion().isEmpty())
    {
        return;
    }

    const auto selectedText = this->getTextInRange(this->getHighlightedRegion());
    const auto searchIn = this->getDocument().getTextBetween(
        { this->getDocument(), this->getHighlightedRegion().getEnd() },
        { this->getDocument(), std::numeric_limits<int>::max(), std::numeric_limits<int>::max() });
    const auto nextDelta = searchIn.indexOf(selectedText);
    if (nextDelta >= 0)
    {
        const auto newStart = this->getHighlightedRegion().getEnd() + nextDelta;
        this->selectRegion({ this->getDocument(), newStart },
            { this->getDocument(), newStart + selectedText.length() });
        this->highlightedToken = selectedText;
    }
}

void ScriptingPlaygroundEditor::selectPrevious()
{
    if (this->getHighlightedRegion().isEmpty())
    {
        return;
    }

    const auto selectedText = this->getTextInRange(this->getHighlightedRegion());
    const auto searchIn = this->getDocument().getTextBetween({},
        { this->getDocument(), this->getHighlightedRegion().getStart() });
    const auto previous = searchIn.lastIndexOf(selectedText);
    if (previous >= 0)
    {
        this->selectRegion({ this->getDocument(), previous },
            { this->getDocument(), previous + selectedText.length() });
        this->highlightedToken = selectedText;
    }
}

void ScriptingPlaygroundEditor::toggleCommentSelection()
{
    this->newTransaction();

    CodeDocument::Position oldSelectionStart(this->selectionStart),
        oldSelectionEnd(this->selectionEnd), oldCaret(this->getCaretPos());
    oldSelectionStart.setPositionMaintained(true);
    oldSelectionEnd.setPositionMaintained(true);
    oldCaret.setPositionMaintained(true);

    bool hasUncommentedLines = false;
    int minLineStartIndex = INT_MAX;
    const auto newLineChars = this->getDocument().getNewLineCharacters();
    const int lineFrom = this->selectionStart.getLineNumber();
    int lineTo = this->selectionEnd.getLineNumber();
    if (lineTo > lineFrom && this->selectionEnd.getIndexInLine() == 0)
    {
        lineTo--;
    }

    for (int i = lineFrom; i <= lineTo; ++i)
    {
        this->moveCaretTo(CodeDocument::Position(this->getDocument(), i, 0), false);
        this->moveCaretToStartOfLine(false);

        const auto lineText = this->getCaretPos().getLineText();
        if (lineText.isEmpty() || lineText.startsWith(newLineChars))
        {
            continue;
        }

        hasUncommentedLines = hasUncommentedLines || (this->getCaretPos().getCharacter() != ';');

        if (this->getCaretPos().getIndexInLine() < minLineStartIndex)
        {
            minLineStartIndex = this->getCaretPos().getIndexInLine();
        }
    }

    for (int i = lineFrom; i <= lineTo; ++i)
    {
        this->moveCaretTo(CodeDocument::Position(this->getDocument(), i, minLineStartIndex), false);
        if (hasUncommentedLines)
        {
            const auto lineText = this->getCaretPos().getLineText();
            if (!lineText.isEmpty() && !lineText.startsWith(newLineChars))
            {
                this->insertTextAtCaret("; ");
            }
        }
        else if (this->getCaretPos().getCharacter() == ';')
        {
            this->deleteForwards(false);
            if (this->getCaretPos().getCharacter() == ' ')
            {
                this->deleteForwards(false);
            }
        }
    }

    if (this->getCaretPos() != oldCaret)
    {
        this->moveCaretTo(oldCaret, false);
    }

    this->setSelection(oldSelectionStart, oldSelectionEnd);
}

void ScriptingPlaygroundEditor::editorViewportPositionChanged()
{
    this->updateErrorRangeBounds();
}

void ScriptingPlaygroundEditor::caretPositionMoved()
{
    this->resetBreakpointPopup();
    this->highlightedToken.clear();
    this->retokenise();
}

bool ScriptingPlaygroundEditor::pasteFromClipboard()
{
    this->newTransaction();
    const auto clip = SystemClipboard::getTextFromClipboard().removeCharacters("\r");
    if (clip.isNotEmpty())
    {
        this->insertText(clip);
    }

    this->newTransaction();
    return true;
}

void ScriptingPlaygroundEditor::codeDocumentTextInserted(const String &s, int startIndex)
{
    this->resetBreakpointPopup();
    this->highlightedToken.clear();

    this->tokeniser->onTextInserted(startIndex, s.length());

    if (auto *parent = this->getParentComponent())
    {
        parent->postCommandMessage(CommandIDs::ScriptEditorReevaluate);
    }
}

void ScriptingPlaygroundEditor::codeDocumentTextDeleted(int startIndex, int endIndex)
{
    this->resetBreakpointPopup();
    this->highlightedToken.clear();

    this->tokeniser->onTextDeleted(startIndex, endIndex);

    if (auto *parent = this->getParentComponent())
    {
        parent->postCommandMessage(CommandIDs::ScriptEditorReevaluate);
    }
}

void ScriptingPlaygroundEditor::resetBreakpointPopup()
{
    this->stopTimer();
    this->popup->hide();
    this->breakpoint = {};
    this->breakpointTokenBounds = {};
}

#pragma endregion

//===----------------------------------------------------------------------===//
// ScriptingPlayground
//===----------------------------------------------------------------------===//

class ScriptingPlaygroundCornerResizer final : public Component
{
public:

    ScriptingPlaygroundCornerResizer()
    {
        this->setOpaque(false);
        this->setAccessible(false);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(true, false);
        this->setMouseCursor(MouseCursor::TopRightCornerResizeCursor);
    }

    void paint(Graphics &g) override
    {
        const auto w = this->getWidth();
        const auto h = this->getHeight();
        constexpr auto lineThickness = 0.75f;

        for (float i = 0.2f; i < 0.8f; i += 0.25f)
        {
            g.setColour(this->colourDark);
            g.drawLine(w * i, lineThickness,
                w - lineThickness, h * (1.f - i), lineThickness);

            g.setColour(this->colourLight);
            g.drawLine((w * i) + lineThickness, lineThickness,
                w - lineThickness, (h * (1.f - i)) - lineThickness, lineThickness);
        }
    }

    void mouseDown(const MouseEvent &e) override
    {
        this->dragger.startDraggingComponent(this, e);
    }

    void mouseDrag(const MouseEvent &e) override
    {
        this->dragger.dragComponent(this, e, nullptr);

        if (auto *parent = dynamic_cast<ScriptingPlayground *>(this->getParentComponent()))
        {
            parent->updateBounds();
            parent->resized();
        }
    }

    void mouseUp(const MouseEvent &e) override
    {
        if (auto *parent = dynamic_cast<ScriptingPlayground *>(this->getParentComponent()))
        {
            parent->resized();
        }
    }

private:

    ComponentDragger dragger;

    const Colour colourDark = findDefaultColour(ColourIDs::Common::borderLineDark);
    const Colour colourLight = findDefaultColour(ColourIDs::Common::borderLineLight);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScriptingPlaygroundCornerResizer)
};

ScriptingPlayground::ScriptingPlayground(ProjectNode &project) noexcept :
    project(project)
{
    this->setComponentID(ComponentIDs::scriptEditor);

    this->shadowUp = make<ShadowUpwards>(ShadowType::Light);
    this->addAndMakeVisible(this->shadowUp.get());
    this->shadowLeft = make<ShadowLeftwards>(ShadowType::Light);
    this->addAndMakeVisible(this->shadowLeft.get());
    this->shadowRight = make<ShadowRightwards>(ShadowType::Light);
    this->addAndMakeVisible(this->shadowRight.get());

    const auto editorConfig = App::Config().getUiFlags()->getScriptEditorSettings();
    Font font(editorConfig.fontSize);
    font.setTypefaceName(editorConfig.fontName);

    this->codeEditor = make<ScriptingPlaygroundEditor>(project.getScriptCodeDocument(),
        make<ScriptTokeniser>());
    this->codeEditor->setFont(font);
    this->addAndMakeVisible(this->codeEditor.get());

    this->cornerResizer = make<ScriptingPlaygroundCornerResizer>();
    this->addAndMakeVisible(this->cornerResizer.get());

    this->outputText = HelioTheme::makeMultiLineTextEditor(false);
    this->addAndMakeVisible(this->outputText.get());
    this->outputText->setMultiLine(false, false);
    this->outputText->setColour(TextEditor::textColourId,
        findDefaultColour(TextEditor::textColourId).withMultipliedAlpha(0.6f));
    const auto codeBg =
        findDefaultColour(CodeEditorComponent::backgroundColourId);
    const auto outputBg = codeBg.brighter(0.025f);
    this->outputText->setColour(TextEditor::backgroundColourId, outputBg);
    this->outputText->setColour(TextEditor::outlineColourId, outputBg);
    this->outputText->setIndents(ScriptingPlayground::iconSize * 2, 0);
    this->outputText->setJustification(Justification::centredLeft);
    this->outputText->setFont(font);

    this->copyOutputButton = make<IconButton>(Icons::copy,
        CommandIDs::ScriptEditorCopyOutput, this, ScriptingPlayground::iconSize);
    this->addChildComponent(this->copyOutputButton.get());
    this->runButton = make<IconButton>(Icons::play,
        CommandIDs::ScriptEditorRunScript, this, ScriptingPlayground::iconSize);
    this->addChildComponent(this->runButton.get());

    this->shadowBottom = make<ShadowUpwards>(ShadowType::Light);
    this->addAndMakeVisible(this->shadowBottom.get());

    this->setSize(editorConfig.size.getX(), editorConfig.size.getY());

    int caretPosition = 0;
    int startLine = 0;
    this->project.getScriptEngine().onEditorOpen(caretPosition, startLine);
    this->codeEditor->configure(caretPosition, startLine);

    this->postCommandMessage(CommandIDs::ScriptEditorReevaluate);

    this->updateOnParse();
    this->updateOnEvaluate();
}

ScriptingPlayground::~ScriptingPlayground()
{
    this->project.getScriptEngine().onEditorClose(
        this->codeEditor->getCaretPos().getPosition(),
        this->codeEditor->getFirstLineOnScreen());
}

void ScriptingPlayground::paint(Graphics &g)
{
    g.setColour(this->frameColour);
    g.fillRect(marginH, marginTop - 1,
        this->getWidth() - marginH * 2,
        this->getHeight() - marginTop + 1);
}

void ScriptingPlayground::resized()
{
    this->shadowUp->setBounds(marginH - 1, 0,
        this->getWidth() - marginH * 2 + 2,
        marginTop);

    this->shadowLeft->setBounds(0, marginTop,
        marginH, this->getHeight() - marginTop);

    this->shadowRight->setBounds(this->getWidth() - marginH, marginTop,
        marginH, this->getHeight() - marginTop);

    constexpr auto statusPanelSize = Globals::UI::sidebarFooterHeight / 2;

    this->codeEditor->setBounds(marginH,
        marginTop,
        this->getWidth() - marginH * 2,
        this->getHeight() - marginTop - statusPanelSize);

    constexpr auto resizerSize = 16;
    this->cornerResizer->setBounds(this->codeEditor->getBounds().
        removeFromRight(resizerSize).removeFromTop(resizerSize));

    Rectangle<int> outputTextBounds(marginH - 1,
        this->codeEditor->getBottom(),
        this->getWidth() - marginH * 2 + 2,
        statusPanelSize);

    this->outputText->setBounds(outputTextBounds);

    this->copyOutputButton->setBounds(
        outputTextBounds.removeFromLeft(ScriptingPlayground::iconSize * 2));

    this->runButton->setBounds(
        outputTextBounds.removeFromRight(ScriptingPlayground::iconSize * 2));

    this->shadowBottom->setBounds(marginH,
        this->codeEditor->getBottom() - 8,
        this->getWidth() - marginH * 2,
        8);
}

void ScriptingPlayground::parentHierarchyChanged()
{
    if (auto *parent = this->getParentComponent())
    {
        const auto parentRelative =
            parent->getBounds().transformedBy(this->getTransform().inverted());
        this->setSize(jmin(this->getWidth(), parentRelative.getWidth()),
            jmin(this->getHeight(), parentRelative.getHeight()));
    }

    this->updatePosition();
}

void ScriptingPlayground::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::ScriptEditorDismiss:
        this->dismiss();
        break;
    case CommandIDs::ScriptEditorRunScript:
        this->project.checkpoint();
        // exit modal state before modifying the project,
        // so that any downstream code dismissing modal components
        // doesn't delete this playground while it's evaluating:
        this->exitModalState(0);
        this->project.getScriptEngine().evaluate(
            this->codeEditor->getDocument().getAllContent(), this, false);
        App::Layout().broadcastCommandMessage(CommandIDs::SwitchToClipInViewport);
        this->enterModalState(true);
        // this->dismiss();
        break;
    case CommandIDs::ScriptEditorReevaluate:
        this->project.getScriptEngine().evaluate(
            this->codeEditor->getDocument().getAllContent(), this, true,
            this->codeEditor->getBreakpoint());
        this->project.sendChangeMessage();
        this->updateOnParse();
        break;
    case CommandIDs::ScriptEditorCopyOutput:
        SystemClipboard::copyTextToClipboard(this->outputText->getText());
        break;
    case CommandIDs::ScriptEditorToggleComment:
        this->codeEditor->toggleCommentSelection();
        break;
    case CommandIDs::ScriptEditorSelectNext:
        this->codeEditor->selectNext();
        break;
    case CommandIDs::ScriptEditorSelectPrevious:
        this->codeEditor->selectPrevious();
        break;
    default:
        break;
    }
}

bool ScriptingPlayground::keyPressed(const KeyPress &key)
{
    App::Config().getHotkeySchemes()->getCurrent()->dispatchKeyPress(key, this, this);
    return true;
}

void ScriptingPlayground::inputAttemptWhenModal()
{
    this->dismiss();
}

void ScriptingPlayground::dismiss()
{
    if (App::isOpenGLRendererEnabled())
    {
        App::animateComponent(this,
            this->getBounds().reduced(10).translated(0, 10),
                0.f, Globals::UI::fadeOutShort, true, 1.0, 0.0);
    }
    else
    {
        App::animateComponent(this, this->getBounds(),
            0.f, Globals::UI::fadeOutShort, true, 1.0, 0.0);
    }

    UniquePointer<Component> deleter(this);
}

Array<KeyPress> ScriptingPlayground::getAllScriptingPlaygroundHotkeys()
{
    static Array<KeyPress> scriptingPlaygroundKeyPresses =
        App::Config().getHotkeySchemes()->getCurrent()->
            findKeyPressesForReceiver(ComponentIDs::scriptEditor);

    return scriptingPlaygroundKeyPresses;
}

void ScriptingPlayground::updateBounds()
{
    if (auto *parent = this->getParentComponent())
    {
        const auto parentRelative =
            parent->getBounds().transformedBy(this->getTransform().inverted());
        const auto minSize =
            App::Config().getUiFlags()->getScriptEditorSettings().minSize.
                transformedBy(this->getTransform().inverted());
        const auto newWidth =
            jmin(parent->getWidth(),
                2 * jmax(minSize.getX() / 2, this->getX() + this->cornerResizer->getX() +
                    this->cornerResizer->getWidth() + marginH - (parentRelative.getWidth() / 2)));
        const auto newHeight =
            jmin(parentRelative.getHeight(),
                jmax(minSize.getY(), parentRelative.getHeight() - this->getY() -
                    this->cornerResizer->getY() + marginTop));
        this->setSize(newWidth, newHeight);
        auto editorConfig = App::Config().getUiFlags()->getScriptEditorSettings();
        editorConfig.size = { newWidth, newHeight };
        App::Config().getUiFlags()->setScriptEditorSettings(editorConfig);
        this->updatePosition();
    }
}

void ScriptingPlayground::updatePosition()
{
    if (auto *parent = this->getParentComponent())
    {
        const auto parentRelative =
            parent->getBounds().transformedBy(this->getTransform().inverted());
        this->setTopLeftPosition(Point<int>(
            (parentRelative.getWidth() / 2) - (this->getWidth() / 2),
            parentRelative.getHeight() - this->getHeight()));
    }
}

void ScriptingPlayground::updateOnParse()
{
    const auto &scriptEngine = this->project.getScriptEngine();
    this->codeEditor->updateParsingData(scriptEngine.getBlockRanges());

    if (scriptEngine.getParsingError().hasValue())
    {
        const auto errorRange = scriptEngine.getParsingError()->sourceCodeRange;

        this->runButton->setVisible(false);
        this->copyOutputButton->setVisible(true);
        this->outputText->setText(scriptEngine.getParsingError()->getDescription());
        this->codeEditor->setError(errorRange, {});
    }
    else if (!scriptEngine.getEvaluationError().hasValue())
    {
        this->codeEditor->setError({}, {});
    }
}

void ScriptingPlayground::updateOnEvaluate()
{
    const auto &scriptEngine = this->project.getScriptEngine();
    this->codeEditor->updateEvaluationData(scriptEngine.getTopLevelFunctionNames());

    if (scriptEngine.getParsingError().hasValue())
    {
        const auto errorText = scriptEngine.getParsingError()->getDescription();
        const auto errorRange = scriptEngine.getParsingError()->sourceCodeRange;

        this->runButton->setVisible(false);
        this->copyOutputButton->setVisible(true);
        this->outputText->setText(errorText);
        this->codeEditor->setError(errorRange, errorText);
    }
    else if (scriptEngine.getEvaluationError().hasValue())
    {
        const auto errorType = scriptEngine.getEvaluationError()->type;
        if (errorType == script::EvaluationError::Type::Aborted)
        {
            return;
        }

        const auto errorText = scriptEngine.getEvaluationError()->getDescription();
        const auto errorRange = scriptEngine.getEvaluationError()->sourceCodeRange;

        if (errorType == script::EvaluationError::Type::BreakpointHit)
        {
            this->codeEditor->setBreakpointInfo(errorText);
        }
        else
        {
            this->runButton->setVisible(false);
            this->copyOutputButton->setVisible(true);
            this->outputText->setText(errorText);
            this->codeEditor->setError(errorRange, errorText);
        }
    }
    else
    {
        this->runButton->setVisible(true);
        this->copyOutputButton->setVisible(true);
        this->outputText->setText("(seed " + String(scriptEngine.random.originalSeed) + ")");
        this->codeEditor->setError({}, {});
    }
}

//===----------------------------------------------------------------------===//
// ScriptEngine::SideEffects
//===----------------------------------------------------------------------===//

void ScriptingPlayground::resetProject()
{
    // let's reset in an undoable way
    while (this->project.findChildOfType<MidiTrackNode>() != nullptr)
    {
        auto *track = this->project.findChildOfType<MidiTrackNode>();
        this->project.removeTrack(*track);
    }

    this->resetTimeline();
}

void ScriptingPlayground::resetTimeline()
{
    auto *ks = this->project.getTimeline()->getKeySignaturesSequence();
    while (ks->size() > 0)
    {
        auto *event = dynamic_cast<KeySignatureEvent *>(ks->getUnchecked(0));
        jassert(event != nullptr);
        ks->remove(*event, true);
    }

    auto *ts = this->project.getTimeline()->getTimeSignaturesSequence();
    while (ts->size() > 0)
    {
        auto *event = dynamic_cast<TimeSignatureEvent *>(ts->getUnchecked(0));
        jassert(event != nullptr);
        ts->remove(*event, true);
    }

    auto *as = this->project.getTimeline()->getAnnotationsSequence();
    while (as->size() > 0)
    {
        auto *event = dynamic_cast<AnnotationEvent *>(as->getUnchecked(0));
        jassert(event != nullptr);
        as->remove(*event, true);
    }
}

void ScriptingPlayground::addKeySignature(const KeySignatureEvent &parameters)
{
    auto *sequence = this->project.getTimeline()->getKeySignaturesSequence();

    // maybe this is a mistake, but I want this method to be smarter:
    // don't add a key if it's the same as the key at the given point:
    int outKey = -1;
    String outKeyName;
    Scale::Ptr outScale;
    const auto beat = parameters.getBeat();
    const auto foundKey =
        SequencerOperations::findHarmonicContext(beat, beat,
            sequence, outScale, outKey, outKeyName);
    if (foundKey &&
        outKey == parameters.getRootKey() &&
        outKeyName == parameters.getRootKeyName() &&
        outScale->isEquivalentTo(parameters.getScale()) &&
        beat > sequence->getFirstBeat())
    {
        //DBG("Skipped adding a key signature");
        return;
    }

    const auto ownedEvent = KeySignatureEvent(sequence, parameters).withNewId();
    sequence->insert(ownedEvent, true);
}

static SerializedData makePianoTrackTemplate(const String &name,
    const String &instrumentId, String &outTrackId)
{
    auto newNode = make<PianoTrackNode>(name);

    const Clip clip(newNode->getPattern(), 0, 0);
    newNode->getPattern()->insert(clip, false);

    Random r;
    const auto colours = ColourIDs::getColoursList();
    const int ci = r.nextInt(colours.size());
    newNode->setTrackColour(colours[ci], false, dontSendNotification);
    newNode->setTrackInstrumentId(instrumentId, false, dontSendNotification);

    outTrackId = newNode->getTrackId();
    return newNode->serialize();
}

String ScriptingPlayground::makePianoTrack(const String &trackName)
{
    // if a track of the same name exists, replace it
    for (auto *track : this->project.getTracks())
    {
        if (track->getTrackName() == trackName)
        {
            this->project.removeTrack(*track);
            break;
        }
    }

    String instrumentId;
    for (auto *instrument : App::Workspace().getAudioCore().getInstrumentsExceptInternal())
    {
        if (trackName.startsWithIgnoreCase(instrument->getName()))
        {
            instrumentId = instrument->getIdAndHash();
            break;
        }
    }

    String trackId;
    const auto trackTemplate =
        makePianoTrackTemplate(trackName, instrumentId, trackId);

    this->project.getUndoStack()->perform(
        new PianoTrackInsertAction(this->project,
            &this->project, trackTemplate, trackName));

    return trackId;
}

MidiTrack *ScriptingPlayground::findPianoTrackById(const String &trackId)
{
    return this->project.findTrackById<PianoTrackNode>(trackId);
}

void ScriptingPlayground::addNotes(MidiTrack *track, Array<Note> &notes, bool undoable)
{
    auto *sequence = dynamic_cast<PianoSequence *>(track->getSequence());
    if (sequence == nullptr)
    {
        jassertfalse;
        return;
    }

    Array<Note> ownedNotes;
    for (const auto &noteParams : notes)
    {
        ownedNotes.add(Note(sequence, noteParams).withNewId());
    }

    sequence->insertGroup(ownedNotes, undoable);
}

void ScriptingPlayground::onProgramTerminated(bool success)
{
    this->updateOnEvaluate();
}

ScriptEngine::SideEffects::ReadOnlyContext ScriptingPlayground::fillHostContext() const
{
    return {
        this->project.getProjectInfo()->getTemperament(),
        this->project.getTimeline()->getKeySignaturesSequence(),
        this->project.getTimeline()->getTimeSignaturesAggregator(),
        App::Config().getScales()->getAll()
    };
}

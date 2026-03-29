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
#include "PianoRoll.h"
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

    ScriptingPlaygroundErrorMark()
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

    static constexpr auto font = Globals::UI::Fonts::XS - 1.f;

    ScriptingPlaygroundPopup()
    {
        this->setOpaque(false);
        this->setAccessible(false);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);

        this->label = make<Label>();
        this->addAndMakeVisible(this->label.get());
        Font f2(ScriptingPlaygroundPopup::font);
        f2.setTypefaceName(Font::getDefaultMonospacedFontName());
        this->label->setFont(f2);
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

ScriptingPlaygroundEditor::ScriptingPlaygroundEditor(CodeDocument &document, CodeTokeniser *codeTokeniser) :
    CodeEditorComponent(document, codeTokeniser)
{
    this->setScrollbarThickness(2);
    this->horizontalScrollBar.setVisible(false);
    this->verticalScrollBar.setColour(ScrollBar::thumbColourId,
        findDefaultColour(CodeEditorComponent::highlightColourId));
    this->setTabSize(2, false); // todo configurable

    this->popup = make<ScriptingPlaygroundPopup>();
    this->addChildComponent(this->popup.get());

    this->errorMark = make<ScriptingPlaygroundErrorMark>();
    this->addChildComponent(this->errorMark.get());

    this->document.addListener(this);
}

ScriptingPlaygroundEditor::~ScriptingPlaygroundEditor()
{
    this->document.removeListener(this);
}

const String &ScriptingPlaygroundEditor::getHighlightedToken() const noexcept
{
    return this->highlightedToken;
}

const Optional<ScriptEngine::Breakpoint> &ScriptingPlaygroundEditor::getBreakpoint() const
{
    return this->breakpoint;
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

void ScriptingPlaygroundEditor::updateErrorRangeBounds()
{
    if (this->errorRange.isEmpty())
    {
        return;
    }

    const CodeDocument::Position start(this->document, this->errorRange.getStart());
    const CodeDocument::Position end(this->document, this->errorRange.getEnd());
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
        document.findLineContaining(position, tokenStart, tokenEnd);
    }
    else
    {
        document.findTokenContaining(position, tokenStart, tokenEnd);
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
            document.findTokenContaining(currentPosition,
                this->selectionAnchorStart, this->selectionAnchorEnd);
        }
        else // selects lines
        {
            document.findLineContaining(currentPosition,
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
        this->highlightedToken =
            this->document.getTextBetween(selectionStart, selectionEnd);

        if (auto *parent = this->getParentComponent())
        {
            parent->postCommandMessage(CommandIDs::ScriptingPlaygroundRetokenise);
        }
    }
    else if (this->highlightedToken.isNotEmpty())
    {
        this->highlightedToken.clear();
        if (auto *parent = this->getParentComponent())
        {
            parent->postCommandMessage(CommandIDs::ScriptingPlaygroundRetokenise);
        }
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
            this->document.getTextBetween(tokenStart, tokenEnd),
            Range<int>(tokenStart.getPosition(), tokenEnd.getPosition()) };

        if (auto *parent = this->getParentComponent())
        {
            parent->postCommandMessage(CommandIDs::ScriptingPlaygroundReevaluate);
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
        findFirstNonWhitespaceChar(this->caretPos.getLineText());

    this->newTransaction();
    this->insertTextAtCaret(document.getNewLineCharacters());

    if (!CharacterFunctions::isWhitespace(this->caretPos.getCharacter()))
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
        this->moveCaretTo(CodeDocument::Position(this->document,
            this->caretPos.getLineNumber(), 0), false);
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
    const auto searchIn = this->document.getTextBetween(
        { this->document, this->getHighlightedRegion().getEnd() },
        { this->document, std::numeric_limits<int>::max(), std::numeric_limits<int>::max() });
    const auto nextDelta = searchIn.indexOf(selectedText);
    if (nextDelta >= 0)
    {
        const auto newStart = this->getHighlightedRegion().getEnd() + nextDelta;
        this->selectRegion({ this->document, newStart },
            { this->document, newStart + selectedText.length() });
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
    const auto searchIn = this->document.getTextBetween({},
        { this->document, this->getHighlightedRegion().getStart() });
    const auto previous = searchIn.lastIndexOf(selectedText);
    if (previous >= 0)
    {
        this->selectRegion({ this->document, previous },
            { this->document, previous + selectedText.length() });
        this->highlightedToken = selectedText;
    }
}

void ScriptingPlaygroundEditor::toggleCommentSelection()
{
    this->newTransaction();

    CodeDocument::Position oldSelectionStart(this->selectionStart),
        oldSelectionEnd(this->selectionEnd), oldCaret(this->caretPos);
    oldSelectionStart.setPositionMaintained(true);
    oldSelectionEnd.setPositionMaintained(true);
    oldCaret.setPositionMaintained(true);

    bool hasUncommentedLines = false;
    int minLineStartIndex = INT_MAX;
    const int lineFrom = this->selectionStart.getLineNumber();
    const int lineTo = this->selectionEnd.getLineNumber();
    for (int i = lineFrom; i <= lineTo; ++i)
    {
        this->moveCaretTo(CodeDocument::Position(this->document, i, 0), false);
        this->moveCaretToStartOfLine(false);

        const auto lineText = this->caretPos.getLineText();
        if (lineText.isEmpty() || lineText.startsWith(newLine))
        {
            continue;
        }

        hasUncommentedLines = hasUncommentedLines || (this->caretPos.getCharacter() != ';');

        if (this->caretPos.getIndexInLine() < minLineStartIndex)
        {
            minLineStartIndex = this->caretPos.getIndexInLine();
        }
    }

    for (int i = lineFrom; i <= lineTo; ++i)
    {
        this->moveCaretTo(CodeDocument::Position(this->document, i, minLineStartIndex), false);
        if (hasUncommentedLines)
        {
            const auto lineText = this->caretPos.getLineText();
            if (!lineText.isEmpty() && !lineText.startsWith(newLine))
            {
                this->insertTextAtCaret("; ");
            }
        }
        else if (this->caretPos.getCharacter() == ';')
        {
            this->deleteForwards(false);
            if (this->caretPos.getCharacter() == ' ')
            {
                this->deleteForwards(false);
            }
        }
    }

    if (this->caretPos != oldCaret)
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

    if (auto *parent = this->getParentComponent())
    {
        parent->postCommandMessage(CommandIDs::ScriptingPlaygroundRetokenise);
    }
}

void ScriptingPlaygroundEditor::codeDocumentTextInserted(const String &s, int startIndex)
{
    this->resetBreakpointPopup();
    this->highlightedToken.clear();

    jassert(dynamic_cast<ScriptTokeniser *>(this->codeTokeniser));
    auto *tokeniser = static_cast<ScriptTokeniser *>(this->codeTokeniser);
    tokeniser->onTextInserted(startIndex, s.length());

    if (auto *parent = this->getParentComponent())
    {
        parent->postCommandMessage(CommandIDs::ScriptingPlaygroundReevaluate);
    }
}

void ScriptingPlaygroundEditor::codeDocumentTextDeleted(int startIndex, int endIndex)
{
    this->resetBreakpointPopup();
    this->highlightedToken.clear();

    jassert(dynamic_cast<ScriptTokeniser *>(this->codeTokeniser));
    auto *tokeniser = static_cast<ScriptTokeniser *>(this->codeTokeniser);
    tokeniser->onTextDeleted(startIndex, endIndex);

    if (auto *parent = this->getParentComponent())
    {
        parent->postCommandMessage(CommandIDs::ScriptingPlaygroundReevaluate);
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

ScriptingPlayground::ScriptingPlayground(ProjectNode &project, RollBase *roll) :
    project(project),
    roll(roll)
{
    this->setComponentID(ComponentIDs::scriptingPlayground);

    this->scriptEngine = make<ScriptEngine>(*this);
    this->tokeniser = make<ScriptTokeniser>();

    this->shadowUp = make<ShadowUpwards>(ShadowType::Light);
    this->addAndMakeVisible(this->shadowUp.get());
    this->shadowLeft = make<ShadowLeftwards>(ShadowType::Light);
    this->addAndMakeVisible(this->shadowLeft.get());
    this->shadowRight = make<ShadowRightwards>(ShadowType::Light);
    this->addAndMakeVisible(this->shadowRight.get());

    this->codeEditor =
        make<ScriptingPlaygroundEditor>(project.getScriptCodeDocument(),
            this->tokeniser.get());
    Font f(Globals::UI::Fonts::S - 1.f); // fixme configurable fonts
    f.setTypefaceName(Font::getDefaultMonospacedFontName());
    this->codeEditor->setFont(f);
    this->addAndMakeVisible(this->codeEditor.get());

    this->cornerResizer = make<ScriptingPlaygroundCornerResizer>();
    this->addAndMakeVisible(this->cornerResizer.get());

    this->outputText = HelioTheme::makeMultiLineTextEditor(false);
    this->addAndMakeVisible(this->outputText.get());
    this->outputText->setMultiLine(false, false);
    this->outputText->setColour(TextEditor::textColourId,
        findDefaultColour(TextEditor::textColourId).withMultipliedAlpha(0.69f));
    const auto codeBg =
        findDefaultColour(CodeEditorComponent::backgroundColourId);
    const auto outputBg = codeBg.brighter(0.025f);
    this->outputText->setColour(TextEditor::backgroundColourId, outputBg);
    this->outputText->setColour(TextEditor::outlineColourId, outputBg);
    this->outputText->setIndents(10, 0);
    this->outputText->setJustification(Justification::centredLeft);
    Font f2(ScriptingPlaygroundPopup::font);
    f2.setTypefaceName(Font::getDefaultMonospacedFontName());
    this->outputText->setFont(f2);

    constexpr auto iconSize = 20;
    this->runButton = make<IconButton>(Icons::play,
        CommandIDs::ScriptingPlaygroundRunScript, this, iconSize);
    this->addChildComponent(this->runButton.get());

    this->shadowBottom = make<ShadowUpwards>(ShadowType::Light);
    this->addAndMakeVisible(this->shadowBottom.get());

    const auto size = App::Config().getUiFlags()->getScriptEditorSize();
    this->setSize(size.getX(), size.getY());

    // fixme should restore the last state instead of this:
    this->postCommandMessage(CommandIDs::ScriptingPlaygroundReevaluate);
}

ScriptingPlayground::~ScriptingPlayground() = default;

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

    this->outputText->setBounds(marginH - 1,
        this->codeEditor->getBottom(),
        this->getWidth() - marginH * 2 + 2,
        statusPanelSize);

    this->runButton->setBounds(
        this->outputText->getBounds().removeFromRight(statusPanelSize));

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

Optional<ScriptEngine::Breakpoint> findBreakpointMinScope(
    const Optional<ScriptEngine::Breakpoint> &from, const Array<Range<int>> &ranges)
{
    if (!from.hasValue())
    {
        return from;
    }

    ScriptEngine::Breakpoint result = *from;
    result.parentListRange = {};
    int minRange = INT_MAX;

    for (const auto &range : ranges)
    {
        if (range.contains(from->parentListRange.getStart()) &&
            range.contains(from->parentListRange.getEnd()))
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

void ScriptingPlayground::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::ScriptingPlaygroundDismiss:
        this->dismiss();
        break;
    case CommandIDs::ScriptingPlaygroundRunScript:
        this->project.checkpoint();
        // exit modal state before modifying the project,
        // so that any downstream code dismissing modal components
        // doesn't delete this playground while it's evaluating:
        this->exitModalState(0);
        this->scriptEngine->evaluate(this->codeEditor->getDocument().getAllContent(), false);
        //this->enterModalState(true);
        this->dismiss();
        break;
    case CommandIDs::ScriptingPlaygroundReevaluate:
        this->scriptEngine->evaluate(this->codeEditor->getDocument().getAllContent(), true,
            findBreakpointMinScope(this->codeEditor->getBreakpoint(),
                this->scriptEngine->getBlockRanges()));
        this->updateOnParse();
        break;
    case CommandIDs::ScriptingPlaygroundRetokenise:
        this->tokeniser->setCaretPosition(this->codeEditor->getCaretPosition());
        this->tokeniser->setHighlightedToken(this->codeEditor->getHighlightedToken());
        this->codeEditor->retokenise(0, 0);
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
            findKeyPressesForReceiver(ComponentIDs::scriptingPlayground);

    return scriptingPlaygroundKeyPresses;
}

void ScriptingPlayground::updateBounds()
{
    if (auto *parent = this->getParentComponent())
    {
        const auto parentRelative =
            parent->getBounds().transformedBy(this->getTransform().inverted());
        const auto minSize =
            Point<int>(690, 690).transformedBy(this->getTransform().inverted());
        const auto newWidth =
            jmin(parent->getWidth(),
                2 * jmax(minSize.getX() / 2, this->getX() + this->cornerResizer->getX() +
                    this->cornerResizer->getWidth() + marginH - (parentRelative.getWidth() / 2)));
        const auto newHeight =
            jmin(parentRelative.getHeight(),
                jmax(minSize.getY(), parentRelative.getHeight() - this->getY() -
                    this->cornerResizer->getY() + marginTop));
        this->setSize(newWidth, newHeight);
        App::Config().getUiFlags()->setScriptEditorSize({ newWidth, newHeight });
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
    if (this->scriptEngine->getParsingError().hasValue())
    {
        const auto errorRange = this->scriptEngine->getParsingError()->sourceCodeRange;

        this->fader.fadeOut(this->runButton.get());
        this->outputText->setText(this->scriptEngine->getParsingError()->getDescription());
        this->tokeniser->setErrorRange(errorRange);
        this->codeEditor->setError(errorRange, {});
    }
    else if (!this->scriptEngine->getEvaluationError().hasValue())
    {
        this->tokeniser->setErrorRange({});
        this->codeEditor->setError({}, {});
    }

    this->tokeniser->updateParsingData(this->scriptEngine->getBlockRanges(),
        this->codeEditor->getCaretPosition());

    this->codeEditor->retokenise(0, 0);
}

void ScriptingPlayground::updateOnEvaluate()
{
    if (this->scriptEngine->getParsingError().hasValue())
    {
        const auto errorText = this->scriptEngine->getParsingError()->getDescription();
        const auto errorRange = this->scriptEngine->getParsingError()->sourceCodeRange;

        this->fader.fadeOut(this->runButton.get());
        this->outputText->setText(errorText);
        this->tokeniser->setErrorRange(errorRange);
        this->codeEditor->setError(errorRange, errorText);
    }
    else if (this->scriptEngine->getEvaluationError().hasValue())
    {
        const auto errorType = this->scriptEngine->getEvaluationError()->type;
        if (errorType == script::EvaluationError::Type::Aborted)
        {
            return;
        }

        const auto errorText = this->scriptEngine->getEvaluationError()->getDescription();
        const auto errorRange = this->scriptEngine->getEvaluationError()->sourceCodeRange;

        if (errorType == script::EvaluationError::Type::BreakpointHit)
        {
            this->codeEditor->setBreakpointInfo(errorText);
        }
        else
        {
            this->fader.fadeOut(this->runButton.get());
            this->outputText->setText(errorText);
            this->tokeniser->setErrorRange(errorRange);
            this->codeEditor->setError(errorRange, errorText);
        }
    }
    else
    {
        this->fader.fadeIn(this->runButton.get());
        const auto &result = this->scriptEngine->getEvaluationResult();
        this->outputText->setText(result.isNil() ? "" : result.toString());
        this->tokeniser->updateEvaluationData(this->scriptEngine->getTopLevelFunctionNames());
        this->tokeniser->setErrorRange({});
        this->codeEditor->setError({}, {});
    }

    this->codeEditor->retokenise(0, 0);
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
        ks->remove(*event, true);
    }

    // fixme time signatures and annotations
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
        outScale->isEquivalentTo(parameters.getScale()))
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

void ScriptingPlayground::addNotes(MidiTrack *track, Array<Note> &notes)
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

    sequence->insertGroup(ownedNotes, true);
}

void ScriptingPlayground::joinAdjacent(MidiTrack *track)
{
    auto *sequence = dynamic_cast<PianoSequence *>(track->getSequence());
    if (sequence == nullptr)
    {
        jassertfalse;
        return;
    }

    SequencerOperations::joinAdjacent(*sequence, true, false);
}

void ScriptingPlayground::arpeggiate(MidiTrack *track, Arpeggiator::Ptr arp)
{
    auto *sequence = dynamic_cast<PianoSequence *>(track->getSequence());
    if (sequence == nullptr)
    {
        jassertfalse;
        return;
    }

    SequencerOperations::arpeggiate(*sequence,
        *track->getPattern()->getClips().getFirst(),
        arp,
        this->project.getProjectInfo()->getTemperament(),
        this->project.getTimeline()->getKeySignaturesSequence(),
        this->project.getTimeline()->getTimeSignaturesAggregator(),
        1.f,    // speed, todo custom
        0.f,    // randomness
        false,  // reversed
        true,   // chord-bound
        true,   // undoable
        false); // shouldCheckpoint
}

void ScriptingPlayground::alignToScale(MidiTrack *track)
{
    auto *sequence = dynamic_cast<PianoSequence *>(track->getSequence());
    if (sequence == nullptr)
    {
        jassertfalse;
        return;
    }

    SequencerOperations::shiftInScaleKeyRelative(*sequence,
        *track->getPattern()->getClips().getFirst(),
        this->project.getTimeline()->getKeySignaturesSequence(),
        this->project.getProjectInfo()->getTemperament()->getHighlighting(), 0,
        true, // undoable
        false); // shouldCheckpoint
}

void ScriptingPlayground::onProgramTerminated(bool success)
{
    this->updateOnEvaluate();
}

ScriptEngine::SideEffects::HostContext ScriptingPlayground::fillHostContext() const
{
    return {
        App::Config().getScales()->getAll(),
        this->project.getProjectInfo()->getTemperament()->getPeriodSize()
    };
}

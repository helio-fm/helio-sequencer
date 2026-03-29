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
#include "ScriptEngine.h"
#include "ScriptTokeniser.h"
#include "PianoRoll.h"
#include "HotkeyScheme.h"
#include "HelioTheme.h"
#include "Config.h"
#include "SerializationKeys.h"
#include "ComponentIDs.h"
#include "CommandIDs.h"

//===----------------------------------------------------------------------===//
// Custom CodeEditorComponent
//===----------------------------------------------------------------------===//

ScriptingPlaygroundEditor::ScriptingPlaygroundEditor(CodeDocument &document, CodeTokeniser *codeTokeniser) :
    CodeEditorComponent(document, codeTokeniser)
{
    this->setScrollbarThickness(2);
    this->setTabSize(2, false); // todo configurable
    this->document.addListener(this);
}

ScriptingPlaygroundEditor::~ScriptingPlaygroundEditor()
{
    this->document.removeListener(this);
}

const Optional<ScriptEngine::Breakpoint> &ScriptingPlaygroundEditor::getBreakpoint() const
{
    return this->breakpoint;
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

    const auto selectionStart = this->getSelectionStart();
    const auto selectionEnd = this->getSelectionEnd();
    if (selectionStart != selectionEnd &&
        selectionStart.getLineNumber() == selectionEnd.getLineNumber())
    {
        const auto selectionText =
            this->document.getTextBetween(selectionStart, selectionEnd);

        this->breakpoint = { selectionText,
            Range<int>(selectionStart.getPosition(), selectionEnd.getPosition()) };

        if (auto *parent = this->getParentComponent())
        {
            parent->postCommandMessage(CommandIDs::ScriptingPlaygroundReevaluate);
        }
    }
    else if (this->breakpoint.hasValue())
    {
        this->breakpoint = {};
        //if (auto *parent = this->getParentComponent())
        //{
        //    parent->postCommandMessage(CommandIDs::ScriptingPlaygroundReevaluate);
        //}
    }
}

void ScriptingPlaygroundEditor::mouseDoubleClick(const MouseEvent &e)
{
    dragType = notDragging;
}

// better indentation for return keys
static int findFirstNonWhitespaceChar(StringRef line) noexcept
{
    int i = 0;
    auto t = line.text;
    while (!t.isEmpty())
    {
        if (!t.isWhitespace()) { return i; }
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

void ScriptingPlaygroundEditor::caretPositionMoved()
{
    if (auto *parent = this->getParentComponent())
    {
        parent->postCommandMessage(CommandIDs::ScriptingPlaygroundRetokenise);
    }
}

void ScriptingPlaygroundEditor::codeDocumentTextInserted(const String &s, int startIndex)
{
    if (auto *parent = this->getParentComponent())
    {
        parent->postCommandMessage(CommandIDs::ScriptingPlaygroundReevaluate);
    }
}

void ScriptingPlaygroundEditor::codeDocumentTextDeleted(int startIndex, int endIndex)
{
    if (auto *parent = this->getParentComponent())
    {
        parent->postCommandMessage(CommandIDs::ScriptingPlaygroundReevaluate);
    }
}

//===----------------------------------------------------------------------===//
// ScriptingPlayground
//===----------------------------------------------------------------------===//

ScriptingPlayground::ScriptingPlayground(ProjectNode &project, RollBase *roll) :
    project(project),
    roll(roll)
{
    this->setComponentID(ComponentIDs::scriptingPlayground);

    this->shadowUp = make<ShadowUpwards>(ShadowType::Normal);
    this->addAndMakeVisible(this->shadowUp.get());
    this->shadowLeft = make<ShadowLeftwards>(ShadowType::Light);
    this->addAndMakeVisible(this->shadowLeft.get());
    this->shadowRight = make<ShadowRightwards>(ShadowType::Light);
    this->addAndMakeVisible(this->shadowRight.get());

    this->scriptEngine = make<ScriptEngine>(*this);

    this->tokeniser = make<ScriptTokeniser>();

    this->codeEditor =
        make<ScriptingPlaygroundEditor>(project.getScriptCodeDocument(),
            this->tokeniser.get());
    Font f(Globals::UI::Fonts::S - 1.f); // fixme configurable fonts
    f.setTypefaceName(Font::getDefaultMonospacedFontName());
    this->codeEditor->setFont(f);
    this->addAndMakeVisible(this->codeEditor.get());

    this->outputText = HelioTheme::makeMultiLineTextEditor(false);
    this->addAndMakeVisible(this->outputText.get());
    this->outputText->setColour(TextEditor::textColourId,
        findDefaultColour(TextEditor::textColourId).withMultipliedAlpha(0.5f));
    const auto codeBg =
        findDefaultColour(CodeEditorComponent::backgroundColourId);
    const auto outputBg = codeBg.brighter(0.015f);
    this->outputText->setColour(TextEditor::backgroundColourId, outputBg);
    this->outputText->setColour(TextEditor::outlineColourId, outputBg);
    this->outputText->setIndents(8, 6);
    Font f2(Globals::UI::Fonts::XS - 1.f); // fixme configurable fonts
    f2.setTypefaceName(Font::getDefaultMonospacedFontName());
    this->outputText->setFont(f2);
    //this->outputText->setText(""); // todo previous results?

    // fixme width not more than parent width
    this->setSize(960, 770); // todo configurable or resizable

    // fixme should restore the last state instead of this:
    this->postCommandMessage(CommandIDs::ScriptingPlaygroundReevaluate);

    // todo transport addListener(this);
}

ScriptingPlayground::~ScriptingPlayground()
{
    // todo transport removeListener(this);
}

void ScriptingPlayground::paint(Graphics &g)
{
    g.setColour(this->frameColour);
    g.fillRect(marginH - 1, marginTop - 1,
        this->getWidth() - marginH * 2 + 2,
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

    constexpr auto outputTextHeight =
        Globals::UI::projectMapHeight - 1; // fixme constant naming

    this->codeEditor->setBounds(marginH,
        marginTop,
        this->getWidth() - marginH * 2,
        this->getHeight() - marginTop - outputTextHeight);

    this->outputText->setBounds(marginH - 1,
        this->codeEditor->getBottom(),
        this->getWidth() - marginH * 2 + 2,
        outputTextHeight);
}

void ScriptingPlayground::parentHierarchyChanged()
{
    this->updatePosition();
}

Optional<ScriptEngine::Breakpoint> findClosestParens(const Array<Range<int>> &ranges,
    const Optional<ScriptEngine::Breakpoint> &from)
{
    if (!from.hasValue())
    {
        return from;
    }

    //DBG("From: " +
    //    String(from->codeBlockRange.getStart()) + ", " +
    //    String(from->codeBlockRange.getEnd()));

    ScriptEngine::Breakpoint result = *from;
    result.parentListRange = {};
    int minRange = INT_MAX;

    for (const auto &range : ranges)
    {
        if (range.contains(from->parentListRange.getStart()) &&
            range.contains(from->parentListRange.getEnd()))
        {
            if (range.getLength() < minRange)
            {
                minRange = range.getLength();
                result.parentListRange = range;
            }
            else
            {
                break;
            }
        }
    }

    //DBG("To: " +
    //    String(result.codeBlockRange.getStart()) + ", " +
    //    String(result.codeBlockRange.getEnd()));

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
            findClosestParens(this->scriptEngine->getBlockRanges(),
                this->codeEditor->getBreakpoint()));
        this->updateOnParse(); // update immediately with the latest parsing data
        break;
    case CommandIDs::ScriptingPlaygroundRetokenise:
        this->tokeniser->setCaretPosition(this->codeEditor->getCaretPosition());
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

void ScriptingPlayground::updatePosition()
{
    if (auto *parent = this->getParentComponent())
    {
        const auto centered = this->getBounds()
            .withCentre(Point<int>(this->getParentWidth() / 2, 0)
            .transformedBy(this->getTransform().inverted()));

        this->setTopLeftPosition(centered.getX(),
            parent->getHeight() - this->getHeight());
    }
}

void ScriptingPlayground::updateOnParse()
{
    if (this->scriptEngine->getParsingError().hasValue())
    {
        this->outputText->setText(this->scriptEngine->getParsingError()->getDescription());
        this->tokeniser->setErrorRange(this->scriptEngine->getParsingError()->sourceCodeRange);
    }
    else
    {
        this->tokeniser->setErrorRange({});
    }

    this->tokeniser->updateParsingData(this->scriptEngine->getBlockRanges(),
        this->codeEditor->getCaretPosition());

    this->codeEditor->retokenise(0, 0);
}

void ScriptingPlayground::updateOnEvaluate()
{
    if (this->scriptEngine->getParsingError().hasValue())
    {
        this->outputText->setText(this->scriptEngine->getParsingError()->getDescription());
        this->tokeniser->setErrorRange(this->scriptEngine->getParsingError()->sourceCodeRange);
    }
    else if (this->scriptEngine->getEvaluationError().hasValue())
    {
        this->outputText->setText(this->scriptEngine->getEvaluationError()->getDescription());
        if (this->scriptEngine->getEvaluationError()->type !=
            script::EvaluationError::Type::BreakpointHit)
        {
            this->tokeniser->setErrorRange(this->scriptEngine->getEvaluationError()->sourceCodeRange);
        }
    }
    else
    {
        const auto &result = this->scriptEngine->getEvaluationResult();
        this->outputText->setText(result.isNil() ? "" : result.toString());
        this->tokeniser->updateEvaluationData(this->scriptEngine->getTopLevelFunctionNames());
        this->tokeniser->setErrorRange({});
    }

    this->codeEditor->retokenise(0, 0);
}

//===----------------------------------------------------------------------===//
// ScriptEngine::SideEffects
//===----------------------------------------------------------------------===//

void ScriptingPlayground::print(const String &output)
{
    // todo
}

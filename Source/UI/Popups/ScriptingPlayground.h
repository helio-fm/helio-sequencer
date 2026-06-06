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

#pragma once

class IconButton;
class RollBase;
class ProjectNode;
class ScriptTokeniser;
class ScriptingPlaygroundPopup;
class ScriptingPlaygroundErrorMark;

#include "ScriptEngine.h"
#include "ComponentFader.h"
#include "ColourIDs.h"

class ScriptingPlaygroundEditor final :
    public CodeEditorComponent,
    private CodeDocument::Listener,
    private Timer // popups on a timeout
{
public:

    ScriptingPlaygroundEditor(CodeDocument &document,
        UniquePointer<ScriptTokeniser> codeTokeniser) noexcept;
    ~ScriptingPlaygroundEditor() override;

    void configure(int defaultCaretPosition,
        int defaultStartLine, int tabSize = 2) noexcept;

    void selectNext();
    void selectPrevious();
    void toggleCommentSelection();

    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseDoubleClick(const MouseEvent &e) override;
    void mouseMove(const MouseEvent &event) override;
    void mouseWheelMove(const MouseEvent &event,
        const MouseWheelDetails &wheel) override;

    void handleReturnKey() override;
    bool keyPressed(const KeyPress &key) override;
    void editorViewportPositionChanged() override;
    void caretPositionMoved() override;
    bool pasteFromClipboard() override;

    const String &getHighlightedToken() const noexcept;
    Optional<ScriptEngine::Breakpoint> getBreakpoint() const;
    void setBreakpointInfo(const String &info);
    void setError(const Range<int> &charRange, const String &text);

    void updateParsingData(const Array<Range<int>> &ranges);
    void updateEvaluationData(const StringArray &functionNames);
    void retokenise();

private:

    UniquePointer<ScriptTokeniser> tokeniser;

    UniquePointer<ScriptingPlaygroundPopup> popup;

    UniquePointer<ScriptingPlaygroundErrorMark> errorMark;
    Range<int> errorRange;
    void updateErrorRangeBounds();

    Array<Range<int>> bracketRanges;

    void timerCallback() override;
    Point<int> lastMouseMovePosition;

    void codeDocumentTextInserted(const String &, int) override;
    void codeDocumentTextDeleted(int, int) override;

    int numFastClicks = 0;
    Time lastMouseDownTime;
    CodeDocument::Position lastMouseDownPosition;
    int lastMouseWheelCounter = 0;

    CodeDocument::Position selectionAnchorStart;
    CodeDocument::Position selectionAnchorEnd;

    Optional<ScriptEngine::Breakpoint> breakpoint;
    Rectangle<int> breakpointTokenBounds;
    void resetBreakpointPopup();

    String highlightedToken;

    void dragSelection(CodeDocument::Position position, bool fullLines = false);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScriptingPlaygroundEditor)
};

class ScriptingPlayground final :
    public Component,
    public TextEditor::Listener,
    public ScriptEngine::SideEffects
{
public:

    explicit ScriptingPlayground(ProjectNode &project) noexcept;
    ~ScriptingPlayground() override;

    static Array<KeyPress> getAllScriptingPlaygroundHotkeys();

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override;
    void resized() override;
    void parentHierarchyChanged() override;
    void handleCommandMessage(int commandId) override;
    bool keyPressed(const KeyPress &key) override;
    void inputAttemptWhenModal() override;

    //===------------------------------------------------------------------===//
    // ScriptEngine::SideEffects
    //===------------------------------------------------------------------===//

    void onProgramTerminated(bool success) override;

    void resetProject() override;
    void resetTimeline() override;
    void addKeySignature(const KeySignatureEvent &event) override;
    String makePianoTrack(const String &trackName) override;
    MidiTrack *findPianoTrackById(const String &trackId) override;
    void addNotes(MidiTrack *track, Array<Note> &notes, bool undoable) override;

    ScriptEngine::SideEffects::ReadOnlyContext fillHostContext() const override;

    void updateBounds();

private:

    void dismiss();
    void updatePosition();

    void updateOnParse();
    void updateOnEvaluate();

    static constexpr int marginH = 8;
    static constexpr int marginTop = 10; // bottom margin is 0

    const Colour frameColour =
        findDefaultColour(ColourIDs::Shadows::borderLight);

private:

    ProjectNode &project;

    UniquePointer<Component> shadowUp;
    UniquePointer<Component> shadowBottom;
    UniquePointer<Component> shadowLeft;
    UniquePointer<Component> shadowRight;
    UniquePointer<Component> cornerResizer;

    UniquePointer<ScriptingPlaygroundEditor> codeEditor;
    UniquePointer<TextEditor> outputText;
    UniquePointer<IconButton> copyOutputButton;
    UniquePointer<IconButton> runButton;

    bool hadErrors = false;

    static constexpr int iconSize = 20;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScriptingPlayground)
};

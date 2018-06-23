/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

//[Headers]
#include "Common.h"
//[/Headers]

#include "ScriptDialog.h"

//[MiscUserDefs]
#include "CommandIDs.h"
//[/MiscUserDefs]

ScriptDialog::ScriptDialog(Component &owner, Pattern *pattern, const Clip &clip)
    : clip(clip),
      pattern(pattern),
      ownerComponent(owner),
      hasMadeChanges(false)
{
    this->background.reset(new DialogPanel());
    this->addAndMakeVisible(background.get());
    this->removeEventButton.reset(new TextButton(String()));
    this->addAndMakeVisible(removeEventButton.get());
    removeEventButton->setButtonText(TRANS("..."));
    removeEventButton->setConnectedEdges (Button::ConnectedOnRight | Button::ConnectedOnTop);
    removeEventButton->addListener(this);

    this->okButton.reset(new TextButton(String()));
    this->addAndMakeVisible(okButton.get());
    okButton->setButtonText(TRANS("..."));
    okButton->setConnectedEdges (Button::ConnectedOnLeft | Button::ConnectedOnTop);
    okButton->addListener(this);

    this->separatorH.reset(new SeparatorHorizontal());
    this->addAndMakeVisible(separatorH.get());
    this->separatorV.reset(new SeparatorVertical());
    this->addAndMakeVisible(separatorV.get());
    this->codeEditor.reset(new CodeEditorComponent(this->document, &this->tokeniser));
    this->addAndMakeVisible(codeEditor.get());


    //[UserPreSize]
    this->document.applyChanges("function transform(int position, int length,\n\
                   int key, int period,\n\
                   float velocity,\n\
                   const float sequenceLength)\n{\n    period += 1;\n    velocity *= 0.75;\n}\n");

    this->codeEditor->setLineNumbersShown(false);
    this->codeEditor->setScrollbarThickness(2);

    this->codeEditor->setFont({ Font::getDefaultMonospacedFontName(), 14.f, Font::plain });
    this->codeEditor->setTabSize(4, true);

    this->okButton->setButtonText(TRANS("dialog::annotation::edit::apply"));
    this->removeEventButton->setButtonText(TRANS("dialog::annotation::edit::delete"));
    this->separatorH->setAlphaMultiplier(2.5f);

    this->document.addListener(this);
    //[/UserPreSize]

    this->setSize(450, 220);

    //[Constructor]
    this->rebound();
    this->setInterceptsMouseClicks(true, true);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->toFront(true);
    this->setAlwaysOnTop(true);
    this->updateOkButtonState();

    this->startTimer(100);
    //[/Constructor]
}

ScriptDialog::~ScriptDialog()
{
    //[Destructor_pre]
    this->document.removeListener(this);
    FadingDialog::fadeOut();
    //[/Destructor_pre]

    background = nullptr;
    removeEventButton = nullptr;
    okButton = nullptr;
    separatorH = nullptr;
    separatorV = nullptr;
    codeEditor = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ScriptDialog::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        float x = 0.0f, y = 0.0f, width = static_cast<float> (getWidth() - 0), height = static_cast<float> (getHeight() - 0);
        Colour fillColour = Colour (0x59000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 10.000f);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ScriptDialog::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds((getWidth() / 2) - ((getWidth() - 8) / 2), 4, getWidth() - 8, getHeight() - 8);
    removeEventButton->setBounds(4, getHeight() - 4 - 48, 220, 48);
    okButton->setBounds(getWidth() - 4 - 221, getHeight() - 4 - 48, 221, 48);
    separatorH->setBounds(4, getHeight() - 52 - 2, getWidth() - 8, 2);
    separatorV->setBounds((getWidth() / 2) - (2 / 2), getHeight() - 4 - 48, 2, 48);
    codeEditor->setBounds((getWidth() / 2) - ((getWidth() - 24) / 2), 12, getWidth() - 24, getHeight() - 72);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ScriptDialog::buttonClicked(Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == removeEventButton.get())
    {
        //[UserButtonCode_removeEventButton] -- add your button handler code here..

        // TODO:
        //this->removeModifier();
        this->disappear();

        //[/UserButtonCode_removeEventButton]
    }
    else if (buttonThatWasClicked == okButton.get())
    {
        //[UserButtonCode_okButton] -- add your button handler code here..
        if (this->document.getNumCharacters() > 0)
        {
            this->disappear();
        }
        //[/UserButtonCode_okButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void ScriptDialog::visibilityChanged()
{
    //[UserCode_visibilityChanged] -- Add your code here...
    //[/UserCode_visibilityChanged]
}

void ScriptDialog::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->rebound();
    //[/UserCode_parentHierarchyChanged]
}

void ScriptDialog::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
    this->rebound();
    //[/UserCode_parentSizeChanged]
}

void ScriptDialog::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::DismissModalDialogAsync)
    {
        this->cancelAndDisappear();
    }
    //[/UserCode_handleCommandMessage]
}

void ScriptDialog::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->postCommandMessage(CommandIDs::DismissModalDialogAsync);
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

void ScriptDialog::updateOkButtonState()
{
    const bool textIsEmpty = this->document.getNumCharacters() == 0;
    this->okButton->setAlpha(textIsEmpty ? 0.5f : 1.f);
    this->okButton->setEnabled(!textIsEmpty);
}

void ScriptDialog::sendClipChange(const Clip &newClip)
{
    if (this->pattern != nullptr)
    {
        this->cancelChangesIfAny();
        this->pattern->checkpoint();
        this->pattern->change(this->clip, newClip, true);
        this->hasMadeChanges = true;
    }
}

void ScriptDialog::cancelChangesIfAny()
{
    if (this->hasMadeChanges && this->pattern != nullptr)
    {
        this->pattern->undo();
        this->hasMadeChanges = false;
    }
}

void ScriptDialog::disappear()
{
    delete this;
}

void ScriptDialog::codeDocumentTextInserted(const String& newText, int insertIndex)
{
    //this->updateOkButtonState();
    //const String text(this->codeEditor->getText());
    //AnnotationEvent newEvent = this->clip.withDescription(text);
    //const Colour c(this->colourSwatches->getColour());
    //this->colourSwatches->setSelectedColour(c);
    //newEvent = newEvent.withColour(c);
    //this->sendEventChange(newEvent);
}

void ScriptDialog::codeDocumentTextDeleted(int startIndex, int endIndex)
{

}

void ScriptDialog::timerCallback()
{
    if (!this->codeEditor->hasKeyboardFocus(false))
    {
        this->codeEditor->grabKeyboardFocus();
        this->codeEditor->selectAll();
        this->stopTimer();
    }
}

void ScriptDialog::cancelAndDisappear()
{
    this->cancelChangesIfAny(); // Discards possible changes
    this->disappear();
}

struct JavaScriptTokeniserFunctions
{
    static bool isReservedKeyword(String::CharPointerType token, const int tokenLength) noexcept
    {
        static const char* const keywords2Char[] =
        { "if", "in", "do", nullptr };

        static const char* const keywords3Char[] =
        { "let", "try", "var", "new", "for", "int", nullptr };

        static const char* const keywords4Char[] =
        { "else", "this", "long", "void", "char", "null", "true", "byte", "enum", "case", "with", nullptr };

        static const char* const keywords5Char[] =
        { "float", "short", "false", "throw", "catch", "while", "break", "const", "final", "super", "class", nullptr };

        static const char* const keywords6Char[] =
        { "switch", "return", "throws", "import", "export", "double", "public", "static", "delete", "native", "typeof", nullptr };

        static const char* const keywords7Char[] =
        { "boolean", "package", "private", "extends", "default", "finally", nullptr };

        static const char* const keywordsOther[] =
        { "continue", "interface", "instanceof", "implements", "protected", "debugger",
            "volatile", "abstract", "function", "transient", nullptr };

        const char* const* k = 0;

        switch(tokenLength)
        {
            case 2: k = keywords2Char; break;
            case 3: k = keywords3Char; break;
            case 4: k = keywords4Char; break;
            case 5: k = keywords5Char; break;
            case 6: k = keywords6Char; break;
            case 7: k = keywords7Char; break;
            default:
            {
                if (tokenLength < 2 || tokenLength > 16) { return false; }
                k = keywordsOther;
                break;
            }
        }

        for (int i = 0; k[i] != 0; ++i)
        {
            if (token.compare(CharPointer_ASCII(k[i])) == 0) { return true; }
        }

        return false;
    }

    template<typename Iterator>
    static int parseIdentifier(Iterator &source) noexcept
    {
        int tokenLength = 0;
        String::CharPointerType::CharType possibleIdentifier[100];
        String::CharPointerType possible(possibleIdentifier);

        while (CppTokeniserFunctions::isIdentifierBody(source.peekNextChar()))
        {
            auto c = source.nextChar();
            if (tokenLength < 20) { possible.write(c); }
            ++tokenLength;
        }

        if (tokenLength > 1 && tokenLength <= 16)
        {
            possible.writeNull();
            if (isReservedKeyword(String::CharPointerType(possibleIdentifier), tokenLength))
            {
                return JavaScriptTokeniser::tokenType_keyword;
            }
        }

        return JavaScriptTokeniser::tokenType_identifier;
    }

    template <typename Iterator>
    static int readNextToken(Iterator& source)
    {
        source.skipWhitespace();

        auto firstChar = source.peekNextChar();

        switch (firstChar)
        {
        case 0:
            break;
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '.':
        {
            auto result = CppTokeniserFunctions::parseNumber(source);
            if (result == JavaScriptTokeniser::tokenType_error)
            {
                source.skip();
                if (firstChar == '.') { return JavaScriptTokeniser::tokenType_punctuation; }
            }

            return result;
        }
        case ',': case ';':  case ':':
            source.skip();
            return JavaScriptTokeniser::tokenType_punctuation;
        case '(': case ')': case '{': case '}': case '[': case ']':
            source.skip();
            return JavaScriptTokeniser::tokenType_bracket;
        case '"': case '\'':
            CppTokeniserFunctions::skipQuotedString(source);
            return JavaScriptTokeniser::tokenType_string;
        case '+':
            source.skip();
            CppTokeniserFunctions::skipIfNextCharMatches(source, '+', '=');
            return JavaScriptTokeniser::tokenType_operator;
        case '-':
        {
            source.skip();
            auto result = CppTokeniserFunctions::parseNumber(source);
            if (result == CPlusPlusCodeTokeniser::tokenType_error)
            {
                CppTokeniserFunctions::skipIfNextCharMatches(source, '-', '=');
                return JavaScriptTokeniser::tokenType_operator;
            }
            return result;
        }
        case '/':
        {
            source.skip();
            auto nextChar = source.peekNextChar();
            if (nextChar == '/')
            {
                source.skipToEndOfLine();
                return JavaScriptTokeniser::tokenType_comment;
            }

            if (nextChar == '*')
            {
                source.skip();
                CppTokeniserFunctions::skipComment(source);
                return JavaScriptTokeniser::tokenType_comment;
            }

            if (nextChar == '=') { source.skip(); }
            return JavaScriptTokeniser::tokenType_operator;
        }
        case '*': case '%': case '=': case '!':
            source.skip();
            CppTokeniserFunctions::skipIfNextCharMatches(source, '=');
            return JavaScriptTokeniser::tokenType_operator;
        case '?': case '~':
            source.skip();
            return JavaScriptTokeniser::tokenType_operator;
        case '<': case '>': case '|': case '&': case '^':
            source.skip();
            CppTokeniserFunctions::skipIfNextCharMatches(source, firstChar);
            CppTokeniserFunctions::skipIfNextCharMatches(source, '=');
            return JavaScriptTokeniser::tokenType_operator;
        default:
            if (CppTokeniserFunctions::isIdentifierStart(firstChar)) { return parseIdentifier(source); }
            source.skip();
            break;
        }

        return JavaScriptTokeniser::tokenType_error;
    }
};

int JavaScriptTokeniser::readNextToken(CodeDocument::Iterator& source)
{
    return JavaScriptTokeniserFunctions::readNextToken(source);
}

CodeEditorComponent::ColourScheme JavaScriptTokeniser::getDefaultColourScheme()
{
    static const CodeEditorComponent::ColourScheme::TokenType types[] =
    {
        { "Error",          Colour(0xffcc0000) },
        { "Comment",        Colour(0xff3c3c3c) },
        { "Keyword",        Colour(0xff0000cc) },
        { "Operator",       Colour(0xff225500) },
        { "Identifier",     Colour(0xff000000) },
        { "Integer",        Colour(0xff880000) },
        { "Float",          Colour(0xff885500) },
        { "String",         Colour(0xff990099) },
        { "Bracket",        Colour(0xff000055) },
        { "Punctuation",    Colour(0xff004400) }
    };

    CodeEditorComponent::ColourScheme cs;
    for (auto &t : types)
    {
        cs.set(t.name, Colour(t.colour));
    }

    return cs;
}



//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ScriptDialog" template="../../Template"
                 componentName="" parentClasses="public FadingDialog, public CodeDocument::Listener, private Timer"
                 constructorParams="Component &amp;owner, Pattern *pattern, const Clip &amp;clip"
                 variableInitialisers="clip(clip),&#10;pattern(pattern),&#10;ownerComponent(owner),&#10;hasMadeChanges(false)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="450" initialHeight="220">
  <METHODS>
    <METHOD name="parentSizeChanged()"/>
    <METHOD name="parentHierarchyChanged()"/>
    <METHOD name="visibilityChanged()"/>
    <METHOD name="inputAttemptWhenModal()"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="0 0 0M 0M" cornerSize="10.00000000000000000000" fill="solid: 59000000"
               hasStroke="0"/>
  </BACKGROUND>
  <JUCERCOMP name="" id="e96b77baef792d3a" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0Cc 4 8M 8M" posRelativeH="ac3897c4f32c4354"
             sourceFile="../Themes/DialogPanel.cpp" constructorParams=""/>
  <TEXTBUTTON name="" id="ccad5f07d4986699" memberName="removeEventButton"
              virtualName="" explicitFocusOrder="0" pos="4 4Rr 220 48" buttonText="..."
              connectedEdges="6" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="7855caa7c65c5c11" memberName="okButton" virtualName=""
              explicitFocusOrder="0" pos="4Rr 4Rr 221 48" buttonText="..."
              connectedEdges="5" needsCallback="1" radioGroupId="0"/>
  <JUCERCOMP name="" id="e39d9e103e2a60e6" memberName="separatorH" virtualName=""
             explicitFocusOrder="0" pos="4 52Rr 8M 2" sourceFile="../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="1fb927654787aaf4" memberName="separatorV" virtualName=""
             explicitFocusOrder="0" pos="0Cc 4Rr 2 48" sourceFile="../Themes/SeparatorVertical.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="6514dc78425c1c39" memberName="codeEditor" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 12 24M 72M" class="CodeEditorComponent"
                    params="this-&gt;document, &amp;this-&gt;tokeniser"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

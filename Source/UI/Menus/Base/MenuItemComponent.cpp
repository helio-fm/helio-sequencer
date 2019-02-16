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

#include "MenuItemComponent.h"

//[MiscUserDefs]

#include "MenuItemComponentMarker.h"
#include "MenuPanel.h"
#include "HelioTheme.h"
#include "IconComponent.h"
#include "Icons.h"
#include "HotkeyScheme.h"
#include "MainLayout.h"
#include "Config.h"

#define ICON_MARGIN (8)
#define MAX_MENU_FONT_SIZE (28.f)

#if JUCE_MAC
#   define HAS_OPENGL_BUG 1
#endif

inline int iconHeightByComponentHeight(int h)
{
    return int(h * 0.6);
}

//===----------------------------------------------------------------------===//
// MenuItem
//===----------------------------------------------------------------------===//

MenuItem::MenuItem() :
    commandId(0), iconId(0), isToggled(false), isDisabled(false),
    shouldCloseMenu(false), hasSubmenu(false), hasTimer(false),
    alignment(Alignment::Left), callback(nullptr) {}

MenuItem::Ptr MenuItem::empty()
{
    MenuItem::Ptr description(new MenuItem());
    description->colour = Colours::transparentBlack;
    return description;
}

inline static String findHotkeyText(int commandId)
{
#if HELIO_DESKTOP
    return App::Config().getHotkeySchemes()->getCurrent()->findHotkeyDescription(commandId);
#elif HELIO_MOBILE
    // Don't show any hotkeys on mobile devices
    return {};
#endif
}

MenuItem::Ptr MenuItem::item(Icons::Id iconId, int commandId, const String &text /*= ""*/)
{
    MenuItem::Ptr description(new MenuItem());
    description->iconId = iconId;
    description->commandText = text;
    description->commandId = commandId;
    description->hotkeyText = findHotkeyText(commandId);
    description->isToggled = false;
    description->isDisabled = false;
    description->shouldCloseMenu = false;
    description->hasSubmenu = false;
    description->hasTimer = false;
    description->colour = findDefaultColour(Label::textColourId);
    return description;
}

MenuItem::Ptr MenuItem::item(Image image, int commandId, const String &text /*= ""*/)
{
    MenuItem::Ptr description(new MenuItem());
    description->image = image;
    description->commandText = text;
    description->commandId = commandId;
    description->hotkeyText = findHotkeyText(commandId);
    description->isToggled = false;
    description->isDisabled = false;
    description->shouldCloseMenu = false;
    description->hasSubmenu = false;
    description->hasTimer = false;
    description->colour = findDefaultColour(Label::textColourId);
    return description;
}

MenuItem::Ptr MenuItem::item(Icons::Id iconId, const String &text)
{
    return MenuItem::item(iconId, -1, text);
}

MenuItem::Ptr MenuItem::withAlignment(Alignment alignment)
{
    MenuItem::Ptr description(this);
    description->alignment = alignment;
    return description;
}

MenuItem::Ptr MenuItem::withSubmenu()
{
    MenuItem::Ptr description(this);
    description->hasSubmenu = true;
    description->hasTimer = true; // a hack
    return description;
}

MenuItem::Ptr MenuItem::withTimer()
{
    MenuItem::Ptr description(this);
    description->hasTimer = true;
    return description;
}

MenuItem::Ptr MenuItem::toggled(bool shouldBeToggled)
{
    MenuItem::Ptr description(this);
    description->isToggled = shouldBeToggled;
    return description;
}

MenuItem::Ptr MenuItem::colouredWith(const Colour &colour)
{
    MenuItem::Ptr description(this);
    description->colour = colour.interpolatedWith(findDefaultColour(Label::textColourId), 0.4f);
    return description;
}

MenuItem::Ptr MenuItem::disabledIf(bool condition)
{
    MenuItem::Ptr description(this);
    description->isDisabled = condition;
    return description;
}

MenuItem::Ptr MenuItem::withAction(const Callback &lambda)
{
    MenuItem::Ptr description(this);
    description->callback = lambda;
    return description;
}

MenuItem::Ptr MenuItem::closesMenu()
{
    MenuItem::Ptr description(this);
    jassert(description->callback == nullptr);
    description->shouldCloseMenu = true;
    return description;
}

//===----------------------------------------------------------------------===//
// Highlighters
//===----------------------------------------------------------------------===//

class ColourHighlighter final : public Component
{
public:

    explicit ColourHighlighter(const Colour &targetColour) :
        colour(targetColour/*.interpolatedWith(Colours::white, 0.5f)*/)
    {
        this->setInterceptsMouseClicks(false, false);
        this->setMouseClickGrabsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);
    }

    void paint(Graphics &g) override
    {
        g.setColour(Colours::black.withAlpha(0.1f));
        g.drawRoundedRectangle(this->getLocalBounds().reduced(4).toFloat(), 5.f, 1.f);

        g.setColour(this->colour.withAlpha(0.1f));
        g.fillRoundedRectangle(this->getLocalBounds().reduced(4).toFloat(), 5);
    }

private:

    const Colour colour;

};

class CommandDragHighlighter final : public Component
{
public:

    CommandDragHighlighter()
    {
        this->setInterceptsMouseClicks(false, false);
        this->setMouseClickGrabsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);
    }

    public: void paint(Graphics &g) override
    {
        g.setColour(Colours::white.withAlpha(0.0175f));
        g.fillRoundedRectangle(this->getLocalBounds().toFloat(), 2.f);
    }
};

class CommandItemSelector final : public Component
{
public:

    CommandItemSelector()
    {
        this->setInterceptsMouseClicks(false, false);
        this->setMouseClickGrabsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);
    }

    void paint(Graphics &g) override
    {
        g.setColour(Colours::white.withAlpha(0.075f));
        g.fillRect(this->getLocalBounds());
    }
};

//[/MiscUserDefs]

MenuItemComponent::MenuItemComponent(Component *parentCommandReceiver, Viewport *parentViewport, const MenuItem::Ptr desc)
    : DraggingListBoxComponent(parentViewport),
      parent(parentCommandReceiver),
      description(MenuItem::empty()),
      mouseDownWasTriggered(false)
{
    addAndMakeVisible (subLabel = new Label (String(),
                                             TRANS("...")));
    subLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    subLabel->setJustificationType (Justification::centredRight);
    subLabel->setEditable (false, false, false);

    addAndMakeVisible (textLabel = new Label (String(),
                                              TRANS("...")));
    textLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    textLabel->setJustificationType (Justification::centredLeft);
    textLabel->setEditable (false, false, false);

    addAndMakeVisible (submenuMarker = new IconComponent (Icons::submenu, 0.25f));


    //[UserPreSize]
    this->toggleMarker = nullptr;
    this->lastMouseScreenPosition = { 0, 0 };
    this->subLabel->setInterceptsMouseClicks(false, false);
    this->textLabel->setInterceptsMouseClicks(false, false);
    this->subLabel->setColour(Label::textColourId, desc->colour.withMultipliedAlpha(0.5f));

    this->setMouseClickGrabsKeyboardFocus(false);
    this->setInterceptsMouseClicks(true, true);
    this->setPaintingIsUnclipped(true);
    //[/UserPreSize]

    setSize (512, 40);

    //[Constructor]
    this->setSize(this->getWidth(), COMMAND_PANEL_BUTTON_HEIGHT);
    this->update(desc);
    //[/Constructor]
}

MenuItemComponent::~MenuItemComponent()
{
    //[Destructor_pre]
    //this->selectionComponent = nullptr;
    //[/Destructor_pre]

    subLabel = nullptr;
    textLabel = nullptr;
    submenuMarker = nullptr;

    //[Destructor]
    //[/Destructor]
}

void MenuItemComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..

    if (this->parentViewport)
    {

    //[/UserPrePaint]

    {
        int x = 0, y = 0, width = getWidth() - 0, height = 1;
        Colour fillColour = Colour (0x06ffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 0, y = getHeight() - 1, width = getWidth() - 0, height = 1;
        Colour fillColour = Colour (0x0f000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..

#if HELIO_DESKTOP

    g.setColour (Colour (0x02ffffff));
    g.fillRect (0, 1, getWidth() - 0, getHeight() - 1);

#endif

    }

    g.setOpacity(1.f);

    const int iconX = this->hasText() ?
        (this->icon.getWidth() / 2) + ICON_MARGIN : (this->getWidth() / 2);

    jassert(this->icon.isValid());
    
    Icons::drawImageRetinaAware(this->icon, g, iconX, this->getHeight() / 2);
    //[/UserPaint]
}

void MenuItemComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    subLabel->setBounds (getWidth() - 4 - 128, (getHeight() / 2) - ((getHeight() - 0) / 2), 128, getHeight() - 0);
    textLabel->setBounds (48, (getHeight() / 2) - ((getHeight() - 0) / 2), getWidth() - 56, getHeight() - 0);
    submenuMarker->setBounds (getWidth() - 4 - 24, (getHeight() / 2) - ((getHeight() - 16) / 2), 24, getHeight() - 16);
    //[UserResized] Add your own custom resize handling here..

    if (!this->icon.isValid() && this->description->image.isValid())
    {
        this->icon = this->description->image;
    }
    else
    {
        this->icon = Icons::findByName(this->description->iconId,
            iconHeightByComponentHeight(this->getHeight()));
    }

    const float xMargin = ICON_MARGIN * 1.2f;
    this->textLabel->setBounds(int(this->icon.getWidth() + xMargin),
        int((this->getHeight() / 2) - (this->getHeight() / 2)),
        int(this->getWidth() - this->icon.getWidth() - xMargin),
        this->getHeight());

    if (this->colourHighlighter != nullptr)
    {
        this->colourHighlighter->setBounds(this->getLocalBounds());
    }

    const float fontSize = jmin(MAX_MENU_FONT_SIZE, float(this->getHeight() / 2) + 1.f);
    this->subLabel->setFont(Font(Font::getDefaultSansSerifFontName(), fontSize, Font::plain));
    this->textLabel->setFont(Font(Font::getDefaultSansSerifFontName(), fontSize, Font::plain));

    //[/UserResized]
}

void MenuItemComponent::mouseMove (const MouseEvent& e)
{
    //[UserCode_mouseMove] -- Add your code here...
    if (this->description->hasTimer &&
        e.getScreenPosition().getDistanceSquaredFrom(this->lastMouseScreenPosition) > 0 &&
        !this->isTimerRunning())
    {
        this->startTimer(650);
    }

    DraggingListBoxComponent::mouseMove(e);

    // This prevents item from being triggered
    // after returning from a submenu, when
    // this item happens to appear under mouse cursor
    this->lastMouseScreenPosition = e.getScreenPosition();
    //[/UserCode_mouseMove]
}

void MenuItemComponent::mouseEnter (const MouseEvent& e)
{
    //[UserCode_mouseEnter] -- Add your code here...
    this->lastMouseScreenPosition = e.getScreenPosition();
    DraggingListBoxComponent::mouseEnter(e);
    //[/UserCode_mouseEnter]
}

void MenuItemComponent::mouseExit (const MouseEvent& e)
{
    //[UserCode_mouseExit] -- Add your code here...
    DraggingListBoxComponent::mouseExit(e);
    this->stopTimer();
    //[/UserCode_mouseExit]
}

void MenuItemComponent::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    if (!this->hasText())
    {
        DraggingListBoxComponent::mouseDown(e);
    }
    else
    {
        if (!this->description->isDisabled)
        {
            this->clickMarker = new CommandDragHighlighter();
            this->addChildComponent(this->clickMarker);
            this->clickMarker->setBounds(this->getLocalBounds());
            this->clickMarker->toBack();

#if HAS_OPENGL_BUG
            this->clickMarker->setVisible(true);
#else
            this->animator.animateComponent(this->clickMarker, this->getLocalBounds(), 1.f, 150, true, 0.0, 0.0);
#endif
        }

        if (this->listCanBeScrolled())
        {
            DraggingListBoxComponent::mouseDown(e);
        }
    }

    this->mouseDownWasTriggered = true;
    //[/UserCode_mouseDown]
}

void MenuItemComponent::mouseUp (const MouseEvent& e)
{
    //[UserCode_mouseUp] -- Add your code here...
    if (! this->mouseDownWasTriggered)
    {
        return;
    }

    if (!this->hasText())
    {
        DraggingListBoxComponent::mouseUp(e);
    }
    else
    {
        if (this->clickMarker)
        {
#if ! HAS_OPENGL_BUG
            this->animator.animateComponent(this->clickMarker, this->getLocalBounds(), 0.f, 100, true, 0.0, 0.0);
#endif

            this->removeChildComponent(this->clickMarker);
            this->clickMarker = nullptr;
        }

        if (this->listCanBeScrolled())
        {
            DraggingListBoxComponent::mouseUp(e);
        }
        else
        {
            if (this->contains(e.getPosition()))
            {
                this->setSelected(true);
            }
        }
    }

    this->mouseDownWasTriggered = false;
    //[/UserCode_mouseUp]
}


//[MiscUserCode]

void MenuItemComponent::setSelected(bool shouldBeSelected)
{
    if (shouldBeSelected && (this->parent != nullptr))
    {
        if (!this->hasText())
        {
#if ! HAS_OPENGL_BUG
            // possible glDeleteTexture bug here?
            ScopedPointer<CommandItemSelector> highlighter(new CommandItemSelector());
            this->addAndMakeVisible(highlighter);
            highlighter->setBounds(this->getLocalBounds());
            this->animator.animateComponent(highlighter, this->getLocalBounds(), 0.f, 200, true, 0.0, 0.0);
            this->removeChildComponent(highlighter);
#endif
        }

        this->doAction();
    }
}

void MenuItemComponent::update(const MenuItem::Ptr desc)
{
    if (this->description->commandText != desc->commandText)
    {
        this->stopTimer();
        this->clearHighlighterAndStopAnimations();
    }

    if (desc->isToggled && !this->description->isToggled)
    {
        this->toggleMarker = new MenuItemComponentMarker();
        this->addAndMakeVisible(this->toggleMarker);
    }
    else if (!desc->isToggled && (this->toggleMarker != nullptr))
    {
        this->toggleMarker = nullptr;
    }

    this->textLabel->setColour(Label::textColourId,
        desc->colour.withMultipliedAlpha(desc->isDisabled ? 0.5f : 1.f));

    this->textLabel->setJustificationType(desc->alignment == MenuItem::Left ?
        Justification::centredLeft : Justification::centredRight);

    this->submenuMarker->setVisible(desc->hasSubmenu);
    this->description = desc;

    if (this->hasText())
    {
        this->textLabel->setText(desc->commandText, dontSendNotification);
        this->textLabel->setVisible(true);
        this->subLabel->setText(desc->hotkeyText, dontSendNotification);
        this->subLabel->setVisible(true);
    }
    else
    {
        this->textLabel->setVisible(false);
        this->subLabel->setVisible(false);
    }

    this->resized();
}

bool MenuItemComponent::hasText() const noexcept
{
    return this->description->commandText.isNotEmpty();
}

Component *MenuItemComponent::createHighlighterComponent()
{
    if (!this->description->isDisabled)
    {
        MenuItem::Ptr desc2 = MenuItem::empty();
        desc2->image = this->description->image;
        desc2->iconId = this->description->iconId;
        desc2->commandText = this->description->commandText;
        //desc2->hotkeyText = this->description->hotkeyText;
        desc2->hasSubmenu = this->description->hasSubmenu;
        desc2->colour = this->description->colour;
        desc2->alignment = this->description->alignment;
        desc2->hasTimer = false;
        return new MenuItemComponent(this->parent, nullptr, desc2);
    }

    return nullptr;
}

void MenuItemComponent::timerCallback()
{
    this->stopTimer();
    jassert(this->description->hasTimer);
    this->doAction();
}

void MenuItemComponent::doAction()
{
    if (this->description->isDisabled)
    {
        return;
    }

    const BailOutChecker checker(this);

    if (this->description->callback != nullptr)
    {
        this->description->callback();
    }

    if (checker.shouldBailOut())
    {
        return;
    }

    if (this->description->commandId > 0)
    {
        if (this->parent != nullptr)
        {
            this->parent->postCommandMessage(this->description->commandId);
        }

        App::Layout().broadcastCommandMessage(this->description->commandId);
    }

    auto panel = dynamic_cast<MenuPanel *>(this->parent.getComponent());
    if (this->description->shouldCloseMenu && panel != nullptr)
    {
        panel->dismiss();
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MenuItemComponent" template="../../../Template"
                 componentName="" parentClasses="public DraggingListBoxComponent, private Timer"
                 constructorParams="Component *parentCommandReceiver, Viewport *parentViewport, const MenuItem::Ptr desc"
                 variableInitialisers="DraggingListBoxComponent(parentViewport),&#10;parent(parentCommandReceiver),&#10;description(MenuItem::empty()),&#10;mouseDownWasTriggered(false)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="512" initialHeight="40">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="mouseUp (const MouseEvent&amp; e)"/>
    <METHOD name="mouseEnter (const MouseEvent&amp; e)"/>
    <METHOD name="mouseExit (const MouseEvent&amp; e)"/>
    <METHOD name="mouseMove (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <RECT pos="0 0 0M 1" fill="solid: 6ffffff" hasStroke="0"/>
    <RECT pos="0 0Rr 0M 1" fill="solid: f000000" hasStroke="0"/>
  </BACKGROUND>
  <LABEL name="" id="8de701891a585730" memberName="subLabel" virtualName=""
         explicitFocusOrder="0" pos="4Rr 0Cc 128 0M" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default sans-serif font"
         fontsize="21.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="34"/>
  <LABEL name="" id="14908053d7863001" memberName="textLabel" virtualName=""
         explicitFocusOrder="0" pos="48 0Cc 56M 0M" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default sans-serif font"
         fontsize="21.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
  <GENERICCOMPONENT name="" id="1e71bff8af38b714" memberName="submenuMarker" virtualName=""
                    explicitFocusOrder="0" pos="4Rr 0Cc 24 16M" class="IconComponent"
                    params="Icons::submenu, 0.25f"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

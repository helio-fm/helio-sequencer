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
#include "UserInterfaceSettings.h"
#include "ModalDialogConfirmation.h"
#include "SerializationKeys.h"
#include "HelioTheme.h"
#include "Config.h"

#if PLATFORM_DESKTOP
#define SIMPLIFIED_UI_SETTINGS 0
#elif PLATFORM_MOBILE
#define SIMPLIFIED_UI_SETTINGS 1
#endif

UserInterfaceSettings::UserInterfaceSettings()
{
    this->setFocusContainerType(Component::FocusContainerType::none);
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);

#if !SIMPLIFIED_UI_SETTINGS

    const String lastUsedFontName = App::Config().getProperty(Serialization::Config::lastUsedFont);

    // deferred menu initialization:
    const auto fontsMenuProvider = [this, lastUsedFontName]()
    {
        MenuPanel::Menu fontsMenu;

        this->systemFonts = Font::findAllTypefaceNames();
        for (int i = 0; i < this->systemFonts.size(); ++i)
        {
            const auto &typefaceName = this->systemFonts.getReference(i);
            const bool isSelected = typefaceName == lastUsedFontName;
            fontsMenu.add(MenuItem::item(isSelected ? Icons::apply : Icons::empty,
                CommandIDs::SelectFont + i, typefaceName));
        }

        return fontsMenu;
    };

    this->fontEditor = HelioTheme::makeSingleLineTextEditor(false);
    this->addAndMakeVisible(this->fontEditor.get());
    this->fontEditor->setText(TRANS(I18n::Settings::uiFont) + ": " + lastUsedFontName);

    this->fontsCombo = make<MobileComboBox::Container>();
    this->addAndMakeVisible(this->fontsCombo.get());
    this->fontsCombo->initWith(this->fontEditor.get(), fontsMenuProvider);
    
    this->openGLRendererButton = make<ToggleButton>(TRANS(I18n::Settings::rendererOpengl));
    this->addAndMakeVisible(this->openGLRendererButton.get());
    this->openGLRendererButton->onClick = [this]()
    {
        const auto newState = this->openGLRendererButton->getToggleState();
        App::Config().getUiFlags()->setOpenGlRendererEnabled(newState);
        this->updateButtons();
    };
    
    this->nativeTitleBarButton = make<ToggleButton>(TRANS(I18n::Settings::nativeTitleBar));
    this->addAndMakeVisible(this->nativeTitleBarButton.get());
    this->nativeTitleBarButton->setToggleState(App::isUsingNativeTitleBar(), dontSendNotification);
    this->nativeTitleBarButton->onClick = [this]()
    {
        // Will reload the layout:
        App::Config().getUiFlags()->setNativeTitleBarEnabled(this->nativeTitleBarButton->getToggleState());
    };

#if JUCE_MAC
    this->nativeTitleBarButton->setEnabled(false);
#endif

    this->wheelFlagsSeparator = make<SeparatorHorizontal>();
    this->addAndMakeVisible(this->wheelFlagsSeparator.get());

    this->wheelAltModeButton = make<ToggleButton>(TRANS(I18n::Settings::mouseWheelPanningByDefault));
    this->addAndMakeVisible(this->wheelAltModeButton.get());
    this->wheelAltModeButton->onClick = [this]()
    {
        App::Config().getUiFlags()->setMouseWheelUsePanningByDefault(this->wheelAltModeButton->getToggleState());
        this->updateButtons();
    };

    this->wheelVerticalPanningButton = make<ToggleButton>(TRANS(I18n::Settings::mouseWheelVerticalPanningByDefault));
    this->addAndMakeVisible(this->wheelVerticalPanningButton.get());
    this->wheelVerticalPanningButton->onClick = [this]()
    {
        App::Config().getUiFlags()->setMouseWheelUseVerticalPanningByDefault(this->wheelVerticalPanningButton->getToggleState());
        this->updateButtons();
    };
    
    this->wheelVerticalZoomingButton = make<ToggleButton>(TRANS(I18n::Settings::mouseWheelVerticalZoomingByDefault));
    this->addAndMakeVisible(this->wheelVerticalZoomingButton.get());
    this->wheelVerticalZoomingButton->onClick = [this]()
    {
        App::Config().getUiFlags()->setMouseWheelUseVerticalZoomingByDefault(this->wheelVerticalZoomingButton->getToggleState());
        this->updateButtons();
    };

    this->miscFlagsSeparator = make<SeparatorHorizontal>();
    this->addAndMakeVisible(this->miscFlagsSeparator.get());

#endif

    this->followPlayheadButton = make<ToggleButton>(TRANS(I18n::Settings::followPlayhead));
    this->addAndMakeVisible(this->followPlayheadButton.get());
    this->followPlayheadButton->onClick = [this]()
    {
        App::Config().getUiFlags()->setFollowingPlayhead(this->followPlayheadButton->getToggleState());
        this->updateButtons();
    };

    this->animationsEnabledButton = make<ToggleButton>(TRANS(I18n::Settings::uiAnimations));
    this->addAndMakeVisible(this->animationsEnabledButton.get());
    this->animationsEnabledButton->onClick = [this]()
    {
        App::Config().getUiFlags()->setUiAnimationsEnabled(this->animationsEnabledButton->getToggleState());
        this->updateButtons();
    };

    this->noteNamesSeparator = make<SeparatorHorizontal>();
    this->addAndMakeVisible(this->noteNamesSeparator.get());

    this->noteNamesTitle = make<Label>(String(), TRANS(I18n::Settings::noteNames));
    this->addAndMakeVisible(this->noteNamesTitle.get());
    this->noteNamesTitle->setFont(Globals::UI::Fonts::M);
    this->noteNamesTitle->setJustificationType(Justification::centredLeft);
    this->noteNamesTitle->setBorderSize({ 0, 2, 0, 2 });
    this->noteNamesTitle->setInterceptsMouseClicks(false, false);

    this->germanNotation = make<ToggleButton>("C, D, E, F, G, A, B");
    this->addAndMakeVisible(this->germanNotation.get());
    this->germanNotation->onClick = [this]() {
        BailOutChecker checker(this);
        App::Config().getUiFlags()->setUseFixedDoNotation(false);
        if (!checker.shouldBailOut())
        {
            this->updateButtons();
        }
    };

    const StringArray fixedDoNames(TRANS(I18n::Solfege::ut),
        TRANS(I18n::Solfege::re), TRANS(I18n::Solfege::mi),
        TRANS(I18n::Solfege::fa), TRANS(I18n::Solfege::sol),
        TRANS(I18n::Solfege::la), TRANS(I18n::Solfege::si));

    this->fixedDoNotation = make<ToggleButton>(fixedDoNames.joinIntoString(", "));
    this->addAndMakeVisible(this->fixedDoNotation.get());
    this->fixedDoNotation->onClick = [this]() {
        BailOutChecker checker(this);
        App::Config().getUiFlags()->setUseFixedDoNotation(true);
        if (!checker.shouldBailOut())
        {
            this->updateButtons();
        }
    };

    this->uiScaleSeparator = make<SeparatorHorizontal>();
    this->addAndMakeVisible(this->uiScaleSeparator.get());

    this->uiScaleTitle = make<Label>(String(), TRANS(I18n::Settings::uiScaling));
    this->addAndMakeVisible(this->uiScaleTitle.get());
    this->uiScaleTitle->setFont(Globals::UI::Fonts::M);
    this->uiScaleTitle->setJustificationType(Justification::centredLeft);
    this->uiScaleTitle->setBorderSize({ 0, 2, 0, 2 });
    this->uiScaleTitle->setInterceptsMouseClicks(false, false);

    this->scaleUi1 = make<ToggleButton>("x1");
    this->addAndMakeVisible(this->scaleUi1.get());
    this->scaleUi1->onClick = [this]() {
        BailOutChecker checker(this);
        App::Config().getUiFlags()->setUiScaleFactor(1.f);
        if (!checker.shouldBailOut())
        {
            this->updateButtons();
        }
    };

    this->scaleUi125 = make<ToggleButton>("x1.25");
    this->addAndMakeVisible(this->scaleUi125.get());
    this->scaleUi125->onClick = [this]() {
        BailOutChecker checker(this);
        App::Config().getUiFlags()->setUiScaleFactor(1.25f);
        if (!checker.shouldBailOut())
        {
            this->updateButtons();
        }
    };

    this->scaleUi15 = make<ToggleButton>("x1.5");
    this->addAndMakeVisible(this->scaleUi15.get());
    this->scaleUi15->onClick = [this]() {
        BailOutChecker checker(this);
        App::Config().getUiFlags()->setUiScaleFactor(1.5f);
        if (!checker.shouldBailOut())
        {
            this->updateButtons();
        }
    };

    this->scaleUi2 = make<ToggleButton>("x2");
    this->addAndMakeVisible(this->scaleUi2.get());
    this->scaleUi2->onClick = [this]() {
        BailOutChecker checker(this);
        App::Config().getUiFlags()->setUiScaleFactor(2.f);
        if (!checker.shouldBailOut())
        {
            this->updateButtons();
        }
    };

#if SIMPLIFIED_UI_SETTINGS
    this->setSize(100, 335);
#else
    this->setSize(100, 545);
#endif
}

UserInterfaceSettings::~UserInterfaceSettings() = default;

void UserInterfaceSettings::resized()
{
    constexpr auto margin1 = 4;
    constexpr auto margin2 = margin1 + 12;
    constexpr auto titleSize = 20;
    constexpr auto rowSize = 26;
    constexpr auto rowSpacing = 4;

#if !SIMPLIFIED_UI_SETTINGS

    this->fontsCombo->setBounds(margin1, margin1,
        this->getWidth() - margin1 * 2, this->getHeight() - margin1 * 2);

    this->fontEditor->setBounds(margin2, margin2,
        this->getWidth() - margin2 * 2, Globals::UI::textEditorHeight);

    this->openGLRendererButton->setBounds(margin2,
        this->fontEditor->getBottom() + (rowSpacing * 2), this->getWidth() - margin2 * 2, rowSize);

    this->nativeTitleBarButton->setBounds(margin2,
        this->openGLRendererButton->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, rowSize);

    this->wheelFlagsSeparator->setBounds(margin2,
        this->nativeTitleBarButton->getBottom() + rowSpacing + 2, this->getWidth() - margin2 * 2, 5);

    this->wheelAltModeButton->setBounds(margin2,
        this->wheelFlagsSeparator->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, rowSize);

    this->wheelVerticalPanningButton->setBounds(margin2,
        this->wheelAltModeButton->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, rowSize);

    this->wheelVerticalZoomingButton->setBounds(margin2,
        this->wheelVerticalPanningButton->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, rowSize);

    this->miscFlagsSeparator->setBounds(margin2,
        this->wheelVerticalZoomingButton->getBottom() + rowSpacing + 2, this->getWidth() - margin2 * 2, 5);

    const auto bottomSectionStart = this->miscFlagsSeparator->getBottom() + rowSpacing;

#else

    const auto bottomSectionStart = margin2;

#endif

    this->followPlayheadButton->setBounds(margin2,
        bottomSectionStart, this->getWidth() - margin2 * 2, rowSize);

    this->animationsEnabledButton->setBounds(margin2,
        this->followPlayheadButton->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, rowSize);

    this->noteNamesSeparator->setBounds(margin2,
        this->animationsEnabledButton->getBottom() + rowSpacing + 2, this->getWidth() - margin2 * 2, 5);

    this->noteNamesTitle->setBounds(margin2,
        this->noteNamesSeparator->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, titleSize);

    this->germanNotation->setBounds(margin2,
        this->noteNamesTitle->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, rowSize);

    this->fixedDoNotation->setBounds(margin2,
        this->germanNotation->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, rowSize);

    this->uiScaleSeparator->setBounds(margin2,
        this->fixedDoNotation->getBottom() + rowSpacing + 2, this->getWidth() - margin2 * 2, 5);

    this->uiScaleTitle->setBounds(margin2,
        this->uiScaleSeparator->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, titleSize);

    this->scaleUi1->setBounds(margin2,
        this->uiScaleTitle->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, rowSize);

    this->scaleUi125->setBounds(margin2,
        this->scaleUi1->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, rowSize);

    this->scaleUi15->setBounds(margin2,
        this->scaleUi125->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, rowSize);

    this->scaleUi2->setBounds(margin2,
        this->scaleUi15->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, rowSize);
}

void UserInterfaceSettings::visibilityChanged()
{
    if (this->isVisible())
    {
        this->updateButtons();
    }
}

void UserInterfaceSettings::handleCommandMessage(int commandId)
{
#if !SIMPLIFIED_UI_SETTINGS
    if (commandId >= CommandIDs::SelectFont &&
        commandId <= (CommandIDs::SelectFont + this->systemFonts.size()))
    {
        const int fontIndex = commandId - CommandIDs::SelectFont;
        auto &theme = static_cast<HelioTheme &>(LookAndFeel::getDefaultLookAndFeel());
        theme.updateFont(Font(this->systemFonts[fontIndex], 0, Font::plain));
        SafePointer<Component> window = this->getTopLevelComponent();
        App::recreateLayout();
        if (window != nullptr)
        {
            window->resized();
            window->repaint();
        }
    }
#endif
}

void UserInterfaceSettings::updateButtons()
{
    const auto *uiFlags = App::Config().getUiFlags();

#if !SIMPLIFIED_UI_SETTINGS
    this->openGLRendererButton->setToggleState(App::isOpenGLRendererEnabled(), dontSendNotification);

    const auto wheelFlags = uiFlags->getMouseWheelFlags();
    this->wheelAltModeButton->setToggleState(wheelFlags.usePanningByDefault, dontSendNotification);
    this->wheelVerticalPanningButton->setToggleState(wheelFlags.useVerticalPanningByDefault, dontSendNotification);
    this->wheelVerticalZoomingButton->setToggleState(wheelFlags.useVerticalZoomingByDefault, dontSendNotification);
#endif

    this->followPlayheadButton->setToggleState(uiFlags->isFollowingPlayhead(), dontSendNotification);
    this->animationsEnabledButton->setToggleState(uiFlags->areUiAnimationsEnabled(), dontSendNotification);

    this->scaleUi1->setToggleState(uiFlags->getUiScaleFactor() == 1.f, dontSendNotification);
    this->scaleUi125->setToggleState(uiFlags->getUiScaleFactor() == 1.25f, dontSendNotification);
    this->scaleUi15->setToggleState(uiFlags->getUiScaleFactor() == 1.5f, dontSendNotification);
    this->scaleUi2->setToggleState(uiFlags->getUiScaleFactor() == 2.f, dontSendNotification);

    this->germanNotation->setToggleState(!uiFlags->isUsingFixedDoNotation(), dontSendNotification);
    this->fixedDoNotation->setToggleState(uiFlags->isUsingFixedDoNotation(), dontSendNotification);
}

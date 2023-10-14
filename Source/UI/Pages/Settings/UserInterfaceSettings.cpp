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

#include "SerializationKeys.h"
#include "ModalDialogConfirmation.h"
#include "HelioTheme.h"
#include "Config.h"

UserInterfaceSettings::UserInterfaceSettings()
{
    this->setFocusContainerType(Component::FocusContainerType::none);
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);

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

    this->fontEditor = make<TextEditor>(String());
    this->addAndMakeVisible(this->fontEditor.get());
    this->fontEditor->setFont(Globals::UI::Fonts::M);
    this->fontEditor->setReadOnly(true);
    this->fontEditor->setScrollbarsShown(false);
    this->fontEditor->setCaretVisible(false);
    this->fontEditor->setPopupMenuEnabled(false);
    this->fontEditor->setInterceptsMouseClicks(false, true);
    this->fontEditor->setText(TRANS(I18n::Settings::uiFont) + ": " + lastUsedFontName);

    this->fontsCombo = make<MobileComboBox::Container>();
    this->addAndMakeVisible(this->fontsCombo.get());
    this->fontsCombo->initWith(this->fontEditor.get(), fontsMenuProvider);
    
    this->openGLRendererButton = make<ToggleButton>(TRANS(I18n::Settings::rendererOpengl));
    this->addAndMakeVisible(this->openGLRendererButton.get());
    this->openGLRendererButton->onClick = [this]()
    {
        if (!this->openGLRendererButton->getToggleState())
        {
            App::Config().getUiFlags()->setOpenGlRendererEnabled(false);
            this->updateButtons();
            return;
        }

        this->openGLRendererButton->setToggleState(false, dontSendNotification);

        auto dialog = ModalDialogConfirmation::Presets::confirmOpenGL();

        dialog->onOk = [this]()
        {
            App::Config().getUiFlags()->setOpenGlRendererEnabled(true);
            this->updateButtons();
        };

        dialog->onCancel = [this]()
        {
            this->updateButtons();
        };

        App::showModalComponent(move(dialog));
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

    this->animationsEnabledButton = make<ToggleButton>(TRANS(I18n::Settings::uiAnimations));
    this->addAndMakeVisible(this->animationsEnabledButton.get());
    this->animationsEnabledButton->onClick = [this]()
    {
        App::Config().getUiFlags()->setUiAnimationsEnabled(this->animationsEnabledButton->getToggleState());
        this->updateButtons();
    };

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

    this->uiScaleSeparator = make<SeparatorHorizontal>();
    this->addAndMakeVisible(this->uiScaleSeparator.get());

    this->scaleUi1 = make<ToggleButton>("x1");
    this->addAndMakeVisible(this->scaleUi1.get());
    this->scaleUi1->onClick = [this]() {
        App::Config().getUiFlags()->setUiScaleFactor(1.f);
        this->updateButtons();
    };

    this->scaleUi15 = make<ToggleButton>("x1.5");
    this->addAndMakeVisible(this->scaleUi15.get());
    this->scaleUi15->onClick = [this]() {
        App::Config().getUiFlags()->setUiScaleFactor(1.5f);
        this->updateButtons();
    };

    this->scaleUi2 = make<ToggleButton>("x2");
    this->addAndMakeVisible(this->scaleUi2.get());
    this->scaleUi2->onClick = [this]() {
        App::Config().getUiFlags()->setUiScaleFactor(2.f);
        this->updateButtons();
    };

    this->setSize(100, 405);
}

UserInterfaceSettings::~UserInterfaceSettings() = default;

void UserInterfaceSettings::resized()
{
    constexpr auto margin1 = 4;
    this->fontsCombo->setBounds(margin1, margin1,
        this->getWidth() - margin1 * 2, this->getHeight() - margin1 * 2);

    constexpr auto margin2 = margin1 + 12;
    constexpr auto rowSize = 32;
    constexpr auto rowSpacing = 4;

    this->fontEditor->setBounds(margin2, margin2, this->getWidth() - margin2 * 2, rowSize);

    this->openGLRendererButton->setBounds(margin2,
        this->fontEditor->getBottom() + margin2, this->getWidth() - margin2 * 2, rowSize);

    this->nativeTitleBarButton->setBounds(margin2,
        this->openGLRendererButton->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, rowSize);

    this->animationsEnabledButton->setBounds(margin2,
        this->nativeTitleBarButton->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, rowSize);

    this->wheelFlagsSeparator->setBounds(margin2,
        this->animationsEnabledButton->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, 4);

    this->wheelAltModeButton->setBounds(margin2,
        this->wheelFlagsSeparator->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, rowSize);

    this->wheelVerticalPanningButton->setBounds(margin2,
        this->wheelAltModeButton->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, rowSize);

    this->wheelVerticalZoomingButton->setBounds(margin2,
        this->wheelVerticalPanningButton->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, rowSize);

    this->uiScaleSeparator->setBounds(margin2,
        this->wheelVerticalZoomingButton->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, 4);

    this->scaleUi1->setBounds(margin2,
        this->uiScaleSeparator->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, rowSize);

    this->scaleUi15->setBounds(margin2,
        this->scaleUi1->getBottom() + rowSpacing, this->getWidth() - margin2 * 2, rowSize);

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
}

// fixme: isn't it better to make this class UserInterfaceFlags::Listener?
void UserInterfaceSettings::updateButtons()
{
    this->openGLRendererButton->setToggleState(App::isOpenGLRendererEnabled(), dontSendNotification);

    const auto *uiFlags = App::Config().getUiFlags();

    const bool hasRollAnimations = uiFlags->areUiAnimationsEnabled();
    this->animationsEnabledButton->setToggleState(hasRollAnimations, dontSendNotification);

    const auto wheelFlags = uiFlags->getMouseWheelFlags();
    this->wheelAltModeButton->setToggleState(wheelFlags.usePanningByDefault, dontSendNotification);
    this->wheelVerticalPanningButton->setToggleState(wheelFlags.useVerticalPanningByDefault, dontSendNotification);
    this->wheelVerticalZoomingButton->setToggleState(wheelFlags.useVerticalZoomingByDefault, dontSendNotification);

    this->scaleUi1->setToggleState(uiFlags->getUiScaleFactor() == 1.f, dontSendNotification);
    this->scaleUi15->setToggleState(uiFlags->getUiScaleFactor() == 1.5f, dontSendNotification);
    this->scaleUi2->setToggleState(uiFlags->getUiScaleFactor() == 2.f, dontSendNotification);
}

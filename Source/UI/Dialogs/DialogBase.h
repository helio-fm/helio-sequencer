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

class DialogBase : public Component
{
public:
    
    DialogBase();
    ~DialogBase() override;

    void paint(Graphics &g) override;
    void parentHierarchyChanged() override;
    void visibilityChanged() override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void inputAttemptWhenModal() override;

protected:
    
    void dismiss();
    virtual void updatePosition();

    Rectangle<int> getContentBounds(bool noPadding = false) const noexcept;
    Rectangle<int> getCaptionBounds() const noexcept;
    Rectangle<int> getRowBounds(float proportionOfHeight, int height, int xPadding = 0) const noexcept;
    Rectangle<int> getButtonsBounds() const noexcept;
    Rectangle<int> getButton1Bounds() const noexcept;
    Rectangle<int> getButton2Bounds() const noexcept;

    int getHorizontalSpacingExceptContent() const noexcept;

protected:

    // on desktop every dialog will try to keep the keyboard focus
    // on the input field, if it has one, but on mobile platforms
    // most dialogs will keep the focus on themselves by default
    // to avoid showing the virtual keyboard immediately
    virtual Component *getPrimaryFocusTarget() { return this; }

    void resetKeyboardFocus()
    {
        if (auto *defaultFocusContainer = this->getPrimaryFocusTarget())
        {
            defaultFocusContainer->grabKeyboardFocus();
        }
    }

    struct Defaults
    {
        // cannot do #ifdefs here: picking a constant
        // depends on the device's physical screen size
        struct Phone
        {
            static constexpr auto buttonsWidth = 128;
            static constexpr auto captionHeight = 0;
            static constexpr auto maxDialogHeight = 140;
        };

        struct DesktopAndTablet
        {
            static constexpr auto buttonsHeight = 48;
            static constexpr auto captionHeight = 38;
        };

        static constexpr auto contentMargin = 1;
        static constexpr auto contentPaddingHorizontal = 16;
        static constexpr auto contentPaddingVertical = 6;
        static constexpr auto textEditorHeight = 32;
    };

private:

    void fadeOut();
    SafePointer<Component> background;

    ComponentDragger dragger;
    UniquePointer<ComponentBoundsConstrainer> moveConstrainer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DialogBase)
};

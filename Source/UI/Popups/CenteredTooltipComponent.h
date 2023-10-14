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

class CenteredTooltipComponent : public Component
{
public:

    CenteredTooltipComponent()
    {
        this->setAlwaysOnTop(true);
        this->setPaintingIsUnclipped(true);
        this->reposition();
    }

    void parentHierarchyChanged() override
    {
        this->reposition();
    }

    void parentSizeChanged() override
    {
        this->reposition();
    }

protected:

    void dismiss()
    {
        auto &animator = Desktop::getInstance().getAnimator();
        if (App::isOpenGLRendererEnabled())
        {
            animator.animateComponent(this, this->getBounds().reduced(20),
                0.f, Globals::UI::fadeOutLong, true, 0.0, 1.0);
        }
        else
        {
            animator.animateComponent(this, this->getBounds(),
                0.f, Globals::UI::fadeOutLong, true, 0.0, 1.0);
        }

        delete this;
    }

private:

    void reposition()
    {
        this->setCentrePosition(this->getParentWidth() / 2, this->getParentHeight() / 2);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CenteredTooltipComponent)
};

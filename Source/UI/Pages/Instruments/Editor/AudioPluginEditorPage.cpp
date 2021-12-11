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

#include "Common.h"
#include "AudioPluginEditorPage.h"
#include "PanelBackgroundB.h"
#include "FramePanel.h"

AudioPluginEditorPage::AudioPluginEditorPage(Component *contentOwned)
{
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);

    this->background = make<PanelBackgroundB>();
    this->addAndMakeVisible(this->background.get());

    this->panel = make<FramePanel>();
    this->addAndMakeVisible(this->panel.get());

    if (auto *w = dynamic_cast<DocumentWindow *>(contentOwned))
    {
        // own the target window
        this->ownedWindow.reset(w);
        this->ownedWindow->setVisible(true);
    }
    else
    {
        this->viewport = make<Viewport>("AudioPluginEditor Viewport");
        this->viewport->setAlwaysOnTop(true);
        this->viewport->setViewedComponent(contentOwned, true);
        this->addAndMakeVisible(this->viewport.get());
    }
}

AudioPluginEditorPage::~AudioPluginEditorPage() = default;

void AudioPluginEditorPage::resized()
{
    this->background->setBounds(0, 0, this->getWidth(), this->getHeight());
    this->panel->setBounds(20, 20, (this->getWidth() - 40), (this->getHeight() - 40));

    if (this->ownedWindow != nullptr)
    {
        const Point<int> g(this->getScreenPosition());
        this->ownedWindow->setBounds(g.getX() + 25, g.getY() + 25, (this->getWidth() - 50), (this->getHeight() - 50));
        this->ownedWindow->toFront(true);
    }

    if (this->viewport != nullptr)
    {
        this->viewport->setBounds(25, 25, (this->getWidth() - 50), (this->getHeight() - 50));
        this->viewport->getViewedComponent()->setSize(this->viewport->getMaximumVisibleWidth(),
            this->viewport->getViewedComponent()->getHeight());
    }
}

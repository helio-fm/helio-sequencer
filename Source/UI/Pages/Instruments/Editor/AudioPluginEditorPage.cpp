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
    this->setWantsKeyboardFocus(false);
    this->setFocusContainer(false);

    this->background = new PanelBackgroundB();
    this->addAndMakeVisible(this->background);

    this->panel = new FramePanel();
    this->addAndMakeVisible(this->panel);

    if (DocumentWindow *w = dynamic_cast<DocumentWindow *>(contentOwned))
    {
        // posess the target window
        this->ownedWindow = w;
        this->ownedWindow->setVisible(true);
    }
    else
    {
        this->viewport = new Viewport("AudioPluginEditor Viewport");
        this->viewport->setAlwaysOnTop(true);
        this->viewport->setViewedComponent(contentOwned, true);
        this->addAndMakeVisible(this->viewport);
    }

    this->setSize(600, 400);
}

AudioPluginEditorPage::~AudioPluginEditorPage()
{
    this->background = nullptr;
    this->panel = nullptr;
    this->viewport = nullptr;
    this->ownedWindow = nullptr;
}

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

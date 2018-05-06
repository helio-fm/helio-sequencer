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
#include "VersionControlEditor.h"
#include "MainLayout.h"
#include "VersionControl.h"

VersionControlEditor::VersionControlEditor(VersionControl &versionControl)
    : vcs(versionControl)
{
    this->setOpaque(true);
    this->setWantsKeyboardFocus(false);
    this->setFocusContainer(false);
}

void VersionControlEditor::broughtToFront()
{
    this->updateState();
}

void VersionControlEditor::changeListenerCallback(ChangeBroadcaster *source)
{
    // VCS or project has changed
    if (this->isShowing())
    {
        this->updateState();
    }
}

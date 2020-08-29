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
#include "PatternEditorNode.h"
#include "TreeNodeSerializer.h"
#include "Icons.h"
#include "MainLayout.h"
#include "ProjectNode.h"
#include "SerializationKeys.h"
#include "PatternsMenu.h"

PatternEditorNode::PatternEditorNode() :
    TreeNode("Patterns", Serialization::Core::patternSet) {}

Image PatternEditorNode::getIcon() const noexcept
{
    return Icons::findByName(Icons::patterns, Globals::UI::headlineIconSize);
}

void PatternEditorNode::showPage()
{
    if (ProjectNode *parentProject =
        this->findParentOfType<ProjectNode>())
    {
        parentProject->showPatternEditor(this);
    }
}

void PatternEditorNode::recreatePage() {}

String PatternEditorNode::getName() const noexcept
{
    return TRANS(I18n::Tree::patterns);
}

//===----------------------------------------------------------------------===//
// Popup
//===----------------------------------------------------------------------===//

bool PatternEditorNode::hasMenu() const noexcept
{
    return true;
}

UniquePointer<Component> PatternEditorNode::createMenu()
{
    return make<PatternsMenu>(*this);
}

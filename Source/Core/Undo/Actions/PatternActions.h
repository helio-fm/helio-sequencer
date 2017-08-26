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

#pragma once

class ProjectTreeItem;

#include "UndoAction.h"
#include "Pattern.h"


//===----------------------------------------------------------------------===//
// Insert Clip
//===----------------------------------------------------------------------===//

class PatternClipInsertAction : public UndoAction
{
public:

    explicit PatternClipInsertAction(ProjectTreeItem &project) :
        UndoAction(project) {}

    PatternClipInsertAction(ProjectTreeItem &project,
        String trackId, const Clip &target);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String trackId;
    Clip clip;

    JUCE_DECLARE_NON_COPYABLE(PatternClipInsertAction)
};


//===----------------------------------------------------------------------===//
// Remove Instance
//===----------------------------------------------------------------------===//

class PatternClipRemoveAction : public UndoAction
{
public:

    explicit PatternClipRemoveAction(ProjectTreeItem &project) :
        UndoAction(project) {}

    PatternClipRemoveAction(ProjectTreeItem &project,
        String trackId, const Clip &target);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String trackId;
    Clip clip;

    JUCE_DECLARE_NON_COPYABLE(PatternClipRemoveAction)
};


//===----------------------------------------------------------------------===//
// Change Instance
//===----------------------------------------------------------------------===//

class PatternClipChangeAction : public UndoAction
{
public:

    explicit PatternClipChangeAction(ProjectTreeItem &project) :
        UndoAction(project) {}

    PatternClipChangeAction(ProjectTreeItem &project,
        String trackId,
        const Clip &target,
        const Clip &newParameters);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String trackId;

    Clip clipBefore;
    Clip clipAfter;

    JUCE_DECLARE_NON_COPYABLE(PatternClipChangeAction)
};

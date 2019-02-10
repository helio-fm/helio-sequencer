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
#include "ProjectInfo.h"
#include "ProjectNode.h"
#include "Delta.h"

using namespace Serialization::VCS;

ProjectInfo::ProjectInfo(ProjectNode &parent) : project(parent)
{
    this->vcsDiffLogic = new VCS::ProjectInfoDiffLogic(*this);

    this->initTimestamp = Time::getCurrentTime().toMilliseconds();
    this->license = "Copyright";
    this->author = SystemStats::getFullUserName();
    this->description = "";

    this->deltas.add(new VCS::Delta({}, ProjectInfoDeltas::projectLicense));
    this->deltas.add(new VCS::Delta({}, ProjectInfoDeltas::projectTitle));
    this->deltas.add(new VCS::Delta({}, ProjectInfoDeltas::projectAuthor));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription("initialized"), ProjectInfoDeltas::projectDescription));
}

int64 ProjectInfo::getStartTimestamp() const         { return this->initTimestamp; }

String ProjectInfo::getLicense() const          { return this->license; }
void ProjectInfo::setLicense(String val)        { this->license = val; this->project.broadcastChangeProjectInfo(this); }

String ProjectInfo::getFullName() const         { return this->project.getName(); }
void ProjectInfo::setFullName(String val)       { this->project.safeRename(val, true); } // will broadcastChangeProjectInfo itself

String ProjectInfo::getAuthor() const           { return this->author; }
void ProjectInfo::setAuthor(String val)         { this->author = val; this->project.broadcastChangeProjectInfo(this); }

String ProjectInfo::getDescription() const      { return this->description; }
void ProjectInfo::setDescription(String val)    { this->description = val; this->project.broadcastChangeProjectInfo(this); }


//===----------------------------------------------------------------------===//
// VCS::TrackedItem
//===----------------------------------------------------------------------===//

String ProjectInfo::getVCSName() const
{
    return ("vcs::items::projectinfo");
}

int ProjectInfo::getNumDeltas() const
{
    return this->deltas.size();
}

VCS::Delta *ProjectInfo::getDelta(int index) const
{
    return this->deltas[index];
}

ValueTree ProjectInfo::getDeltaData(int deltaIndex) const
{
    if (this->deltas[deltaIndex]->hasType(ProjectInfoDeltas::projectLicense))
    {
        return this->serializeLicenseDelta();
    }
    if (this->deltas[deltaIndex]->hasType(ProjectInfoDeltas::projectTitle))
    {
        return this->serializeFullNameDelta();
    }
    else if (this->deltas[deltaIndex]->hasType(ProjectInfoDeltas::projectAuthor))
    {
        return this->serializeAuthorDelta();
    }
    else if (this->deltas[deltaIndex]->hasType(ProjectInfoDeltas::projectDescription))
    {
        return this->serializeDescriptionDelta();
    }

    jassertfalse;
    return {};
}

VCS::DiffLogic *ProjectInfo::getDiffLogic() const
{
    return this->vcsDiffLogic;
}

void ProjectInfo::resetStateTo(const TrackedItem &newState)
{
    for (int i = 0; i < newState.getNumDeltas(); ++i)
    {
        const VCS::Delta *newDelta = newState.getDelta(i);
        const auto newDeltaData(newState.getDeltaData(i));
        
        if (newDelta->hasType(ProjectInfoDeltas::projectLicense))
        {
            this->resetLicenseDelta(newDeltaData);
        }
        else if (newDelta->hasType(ProjectInfoDeltas::projectTitle))
        {
            this->resetFullNameDelta(newDeltaData);
        }
        else if (newDelta->hasType(ProjectInfoDeltas::projectAuthor))
        {
            this->resetAuthorDelta(newDeltaData);
        }
        else if (newDelta->hasType(ProjectInfoDeltas::projectDescription))
        {
            this->resetDescriptionDelta(newDeltaData);
        }
    }
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree ProjectInfo::serialize() const
{
    ValueTree tree(Serialization::Core::projectInfo);

    this->serializeVCSUuid(tree);

    tree.setProperty(Serialization::Core::projectTimeStamp, String(this->initTimestamp), nullptr);
    tree.setProperty(ProjectInfoDeltas::projectLicense, this->getLicense(), nullptr);
    tree.setProperty(ProjectInfoDeltas::projectAuthor, this->getAuthor(), nullptr);
    tree.setProperty(ProjectInfoDeltas::projectDescription, this->getDescription(), nullptr);

    return tree;
}

void ProjectInfo::deserialize(const ValueTree &tree)
{
    this->reset();

    const auto root = tree.hasType(Serialization::Core::projectInfo) ?
        tree : tree.getChildWithName(Serialization::Core::projectInfo);

    if (!root.isValid()) { return; }

    this->deserializeVCSUuid(root);

    this->initTimestamp = root.getProperty(Serialization::Core::projectTimeStamp);
    this->license = root.getProperty(ProjectInfoDeltas::projectLicense);
    this->author = root.getProperty(ProjectInfoDeltas::projectAuthor);
    this->description = root.getProperty(ProjectInfoDeltas::projectDescription);

    this->project.broadcastChangeProjectInfo(this);
}

void ProjectInfo::reset()
{
    this->author.clear();
    this->description.clear();
    this->license.clear();
    this->initTimestamp = 0;
}


//===----------------------------------------------------------------------===//
// Deltas
//===----------------------------------------------------------------------===//

ValueTree ProjectInfo::serializeLicenseDelta() const
{
    ValueTree tree(ProjectInfoDeltas::projectLicense);
    tree.setProperty(Serialization::VCS::delta, this->getLicense(), nullptr);
    return tree;
}

ValueTree ProjectInfo::serializeFullNameDelta() const
{
    ValueTree tree(ProjectInfoDeltas::projectTitle);
    tree.setProperty(Serialization::VCS::delta, this->getFullName(), nullptr);
    return tree;
}

ValueTree ProjectInfo::serializeAuthorDelta() const
{
    ValueTree tree(ProjectInfoDeltas::projectAuthor);
    tree.setProperty(Serialization::VCS::delta, this->getAuthor(), nullptr);
    return tree;
}

ValueTree ProjectInfo::serializeDescriptionDelta() const
{
    ValueTree tree(ProjectInfoDeltas::projectDescription);
    tree.setProperty(Serialization::VCS::delta, this->getDescription(), nullptr);
    return tree;
}

void ProjectInfo::resetLicenseDelta(const ValueTree &state)
{
    jassert(state.hasType(ProjectInfoDeltas::projectLicense));
    const String &licenseDelta = state.getProperty(Serialization::VCS::delta);
    if (licenseDelta != this->license)
    {
        this->setLicense(licenseDelta);
    }
}

void ProjectInfo::resetFullNameDelta(const ValueTree &state)
{
    jassert(state.hasType(ProjectInfoDeltas::projectTitle));
    const String &nameDelta = state.getProperty(Serialization::VCS::delta);
    if (nameDelta != this->getFullName())
    {
        this->setFullName(nameDelta);
    }
}

void ProjectInfo::resetAuthorDelta(const ValueTree &state)
{
    jassert(state.hasType(ProjectInfoDeltas::projectAuthor));
    const String &authorDelta = state.getProperty(Serialization::VCS::delta);
    if (authorDelta != this->author)
    {
        this->setAuthor(authorDelta);
    }
}

void ProjectInfo::resetDescriptionDelta(const ValueTree &state)
{
    jassert(state.hasType(ProjectInfoDeltas::projectDescription));
    const String &descriptionDelta = state.getProperty(Serialization::VCS::delta);
    if (descriptionDelta != this->description)
    {
        this->setDescription(descriptionDelta);
    }
}

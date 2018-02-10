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
#include "ProjectInfoDeltas.h"
#include "ProjectTreeItem.h"
#include "Delta.h"

ProjectInfo::ProjectInfo(ProjectTreeItem &parent) : project(parent)
{
    this->vcsDiffLogic = new VCS::ProjectInfoDiffLogic(*this);

    this->initTimestamp = Time::getCurrentTime().toMilliseconds();
    this->license = "Copyright";
    //this->fullName = this->project.getName();
    this->author = SystemStats::getFullUserName();
    this->description = "";

    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), ProjectInfoDeltas::projectLicense));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), ProjectInfoDeltas::projectFullName));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), ProjectInfoDeltas::projectAuthor));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription("initialized"), ProjectInfoDeltas::projectDescription));
}


ProjectInfo::~ProjectInfo()
{

}

int64 ProjectInfo::getStartTime() const         { return this->initTimestamp; }

String ProjectInfo::getLicense() const          { return this->license; }
void ProjectInfo::setLicense(String val)        { this->license = val; this->project.broadcastChangeProjectInfo(this); }

String ProjectInfo::getFullName() const         { return this->project.getName(); }
void ProjectInfo::setFullName(String val)       { this->project.safeRename(val); this->project.broadcastChangeProjectInfo(this); } // this->fullName = val;

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

XmlElement *ProjectInfo::createDeltaDataFor(int index) const
{
    if (this->deltas[index]->getType() == ProjectInfoDeltas::projectLicense)
    {
        return this->serializeLicenseDelta();
    }
    if (this->deltas[index]->getType() == ProjectInfoDeltas::projectFullName)
    {
        return this->serializeFullNameDelta();
    }
    else if (this->deltas[index]->getType() == ProjectInfoDeltas::projectAuthor)
    {
        return this->serializeAuthorDelta();
    }
    else if (this->deltas[index]->getType() == ProjectInfoDeltas::projectDescription)
    {
        return this->serializeDescriptionDelta();
    }

    jassertfalse;
    return nullptr;
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
        ScopedPointer<XmlElement> newDeltaData(newState.createDeltaDataFor(i));
        
        if (newDelta->getType() == ProjectInfoDeltas::projectLicense)
        {
            this->resetLicenseDelta(newDeltaData);
        }
        else if (newDelta->getType() == ProjectInfoDeltas::projectFullName)
        {
            this->resetFullNameDelta(newDeltaData);
        }
        else if (newDelta->getType() == ProjectInfoDeltas::projectAuthor)
        {
            this->resetAuthorDelta(newDeltaData);
        }
        else if (newDelta->getType() == ProjectInfoDeltas::projectDescription)
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

    this->serializeVCSUuid(*xml);

    tree.setProperty(Serialization::Core::projectTimeStamp, String(this->initTimestamp));
    tree.setProperty(ProjectInfoDeltas::projectLicense, this->getLicense());
//    tree.setProperty(ProjectInfoDeltas::projectFullName, this->getFullName());
    tree.setProperty(ProjectInfoDeltas::projectAuthor, this->getAuthor());
    tree.setProperty(ProjectInfoDeltas::projectDescription, this->getDescription());

    return tree;
}

void ProjectInfo::deserialize(const ValueTree &tree)
{
    this->reset();

    const XmlElement *root = tree.hasTagName(Serialization::Core::projectInfo) ?
                             &tree : tree.getChildByName(Serialization::Core::projectInfo);

    if (root == nullptr) { return; }

    this->deserializeVCSUuid(*root);

    this->initTimestamp = root->getStringAttribute(Serialization::Core::projectTimeStamp).getLargeIntValue();
    this->license = root->getStringAttribute(ProjectInfoDeltas::projectLicense);
//    this->fullName = root->getStringAttribute(ProjectInfoDeltas::projectFullName);
    this->author = root->getStringAttribute(ProjectInfoDeltas::projectAuthor);
    this->description = root->getStringAttribute(ProjectInfoDeltas::projectDescription);

    this->project.broadcastChangeProjectInfo(this);
}

void ProjectInfo::reset()
{
//    this->fullName = "";
    this->author = "";
    this->description = "";
}


//===----------------------------------------------------------------------===//
// Deltas
//===----------------------------------------------------------------------===//

XmlElement *ProjectInfo::serializeLicenseDelta() const
{
    ValueTree tree(ProjectInfoDeltas::projectLicense);
    tree.setProperty(Serialization::VCS::delta, this->getLicense());
    return tree;
}

XmlElement *ProjectInfo::serializeFullNameDelta() const
{
    ValueTree tree(ProjectInfoDeltas::projectFullName);
    tree.setProperty(Serialization::VCS::delta, this->getFullName());
    return tree;
}

XmlElement *ProjectInfo::serializeAuthorDelta() const
{
    ValueTree tree(ProjectInfoDeltas::projectAuthor);
    tree.setProperty(Serialization::VCS::delta, this->getAuthor());
    return tree;
}

XmlElement *ProjectInfo::serializeDescriptionDelta() const
{
    ValueTree tree(ProjectInfoDeltas::projectDescription);
    tree.setProperty(Serialization::VCS::delta, this->getDescription());
    return tree;
}

void ProjectInfo::resetLicenseDelta(const XmlElement *state)
{
    jassert(state->getTagName() == ProjectInfoDeltas::projectLicense);
    const String delta(state->getStringAttribute(Serialization::VCS::delta));

    if (delta != this->license)
    {
        this->setLicense(delta);
    }
}

void ProjectInfo::resetFullNameDelta(const XmlElement *state)
{
    jassert(state->getTagName() == ProjectInfoDeltas::projectFullName);
    const String delta(state->getStringAttribute(Serialization::VCS::delta));

    if (delta != this->getFullName())
    {
        this->setFullName(delta);
    }
}

void ProjectInfo::resetAuthorDelta(const XmlElement *state)
{
    jassert(state->getTagName() == ProjectInfoDeltas::projectAuthor);
    const String delta(state->getStringAttribute(Serialization::VCS::delta));

    if (delta != this->author)
    {
        this->setAuthor(delta);
    }
}

void ProjectInfo::resetDescriptionDelta(const XmlElement *state)
{
    jassert(state->getTagName() == ProjectInfoDeltas::projectDescription);
    const String delta(state->getStringAttribute(Serialization::VCS::delta));

    if (delta != this->description)
    {
        this->setDescription(delta);
    }
}

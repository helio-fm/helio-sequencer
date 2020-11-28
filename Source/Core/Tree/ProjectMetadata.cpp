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
#include "ProjectMetadata.h"
#include "ProjectNode.h"

ProjectMetadata::ProjectMetadata(ProjectNode &parent) : project(parent)
{
    using namespace Serialization::VCS;

    this->vcsDiffLogic = make<VCS::ProjectInfoDiffLogic>(*this);

    this->initTimestamp = Time::getCurrentTime().toMilliseconds();
    this->license = "Copyright";
    this->author = SystemStats::getFullUserName();
    this->description = "";
    this->temperament = Temperament::getTwelveToneEqualTemperament();

    this->deltas.add(new VCS::Delta({}, ProjectInfoDeltas::projectLicense));
    this->deltas.add(new VCS::Delta({}, ProjectInfoDeltas::projectTitle));
    this->deltas.add(new VCS::Delta({}, ProjectInfoDeltas::projectAuthor));
    this->deltas.add(new VCS::Delta({}, ProjectInfoDeltas::projectTemperament));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription("initialized"), ProjectInfoDeltas::projectDescription));
}

int64 ProjectMetadata::getStartTimestamp() const noexcept
{
    return this->initTimestamp;
}

String ProjectMetadata::getLicense() const noexcept
{
    return this->license;
}

void ProjectMetadata::setLicense(String val)
{
    if (this->license != val)
    {
        this->license = val;
        this->project.broadcastChangeProjectInfo(this);
    }
}

String ProjectMetadata::getFullName() const noexcept
{
    return this->project.getName();
}

void ProjectMetadata::setFullName(String val)
{
    this->project.safeRename(val, true); // will broadcastChangeProjectInfo
}

String ProjectMetadata::getAuthor() const noexcept
{
    return this->author;
}

void ProjectMetadata::setAuthor(String val)
{
    if (this->author != val)
    {
        this->author = val;
        this->project.broadcastChangeProjectInfo(this);
    }
}

String ProjectMetadata::getDescription() const noexcept
{
    return this->description;
}

void ProjectMetadata::setDescription(String val)
{
    if (this->description != val)
    {
        this->description = val;
        this->project.broadcastChangeProjectInfo(this);
    }
}

Temperament::Ptr ProjectMetadata::getTemperament() const noexcept
{
    return this->temperament;
}

void ProjectMetadata::setTemperament(const Temperament &value)
{
    if (this->temperament->getResourceId() != value.getResourceId())
    {
        this->temperament = Temperament::Ptr(new Temperament(value));
        this->project.broadcastChangeProjectInfo(this);
    }
}

int ProjectMetadata::getKeyboardSize() const noexcept
{
    return this->temperament->getNumKeys();
}

int ProjectMetadata::getPeriodSize() const noexcept
{
    return this->temperament->getPeriodSize();
}

//===----------------------------------------------------------------------===//
// VCS::TrackedItem
//===----------------------------------------------------------------------===//

String ProjectMetadata::getVCSName() const
{
    return I18n::VCS::projectMetadata.toString();
}

int ProjectMetadata::getNumDeltas() const
{
    return this->deltas.size();
}

VCS::Delta *ProjectMetadata::getDelta(int index) const
{
    return this->deltas[index];
}

SerializedData ProjectMetadata::getDeltaData(int deltaIndex) const
{
    using namespace Serialization::VCS;

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
    else if (this->deltas[deltaIndex]->hasType(ProjectInfoDeltas::projectTemperament))
    {
        return this->serializeTemperamentDelta();
    }

    jassertfalse;
    return {};
}

bool ProjectMetadata::deltaHasDefaultData(int deltaIndex) const
{
    using namespace Serialization::VCS;
    if (this->deltas[deltaIndex]->hasType(ProjectInfoDeltas::projectTemperament))
    {
        jassert(this->temperament != nullptr);
        return this->temperament->isDefault();
    }

    return false;
}

VCS::DiffLogic *ProjectMetadata::getDiffLogic() const
{
    return this->vcsDiffLogic.get();
}

void ProjectMetadata::resetStateTo(const TrackedItem &newState)
{
    using namespace Serialization::VCS;

    // first, reset default state for the new delta, which may be missing in base:
    this->temperament = Temperament::getTwelveToneEqualTemperament();

    for (int i = 0; i < newState.getNumDeltas(); ++i)
    {
        const auto *newDelta = newState.getDelta(i);
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
        else if (newDelta->hasType(ProjectInfoDeltas::projectTemperament))
        {
            this->resetTemperamentDelta(newDeltaData);
        }
    }
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData ProjectMetadata::serialize() const
{
    using namespace Serialization::VCS;

    SerializedData tree(Serialization::Core::projectInfo);

    this->serializeVCSUuid(tree);

    tree.setProperty(Serialization::Core::projectTimeStamp, String(this->initTimestamp));
    tree.setProperty(ProjectInfoDeltas::projectLicense, this->getLicense());
    tree.setProperty(ProjectInfoDeltas::projectAuthor, this->getAuthor());
    tree.setProperty(ProjectInfoDeltas::projectDescription, this->getDescription());

    tree.appendChild(this->temperament->serialize());

    return tree;
}

void ProjectMetadata::deserialize(const SerializedData &data)
{
    using namespace Serialization::VCS;

    this->reset();

    const auto root = data.hasType(Serialization::Core::projectInfo) ?
        data : data.getChildWithName(Serialization::Core::projectInfo);

    if (!root.isValid()) { return; }

    this->deserializeVCSUuid(root);

    this->initTimestamp = root.getProperty(Serialization::Core::projectTimeStamp);
    this->license = root.getProperty(ProjectInfoDeltas::projectLicense);
    this->author = root.getProperty(ProjectInfoDeltas::projectAuthor);
    this->description = root.getProperty(ProjectInfoDeltas::projectDescription);

    if (root.getNumChildren() > 0)
    {
        jassert(this->temperament != nullptr);
        this->temperament->deserialize(root);
    }
    else
    {
        // do we really need to fallback to the default one here?
        this->temperament = Temperament::getTwelveToneEqualTemperament();
    }

    this->project.broadcastChangeProjectInfo(this);
}

void ProjectMetadata::reset()
{
    this->author.clear();
    this->description.clear();
    this->license.clear();
    this->initTimestamp = 0;
}

//===----------------------------------------------------------------------===//
// Deltas
//===----------------------------------------------------------------------===//

SerializedData ProjectMetadata::serializeLicenseDelta() const
{
    SerializedData tree(Serialization::VCS::ProjectInfoDeltas::projectLicense);
    tree.setProperty(Serialization::VCS::delta, this->getLicense());
    return tree;
}

SerializedData ProjectMetadata::serializeFullNameDelta() const
{
    SerializedData tree(Serialization::VCS::ProjectInfoDeltas::projectTitle);
    tree.setProperty(Serialization::VCS::delta, this->getFullName());
    return tree;
}

SerializedData ProjectMetadata::serializeAuthorDelta() const
{
    SerializedData tree(Serialization::VCS::ProjectInfoDeltas::projectAuthor);
    tree.setProperty(Serialization::VCS::delta, this->getAuthor());
    return tree;
}

SerializedData ProjectMetadata::serializeDescriptionDelta() const
{
    SerializedData tree(Serialization::VCS::ProjectInfoDeltas::projectDescription);
    tree.setProperty(Serialization::VCS::delta, this->getDescription());
    return tree;
}

SerializedData ProjectMetadata::serializeTemperamentDelta() const
{
    SerializedData tree(Serialization::VCS::ProjectInfoDeltas::projectTemperament);
    tree.appendChild(this->temperament->serialize());
    return tree;
}

void ProjectMetadata::resetLicenseDelta(const SerializedData &state)
{
    jassert(state.hasType(Serialization::VCS::ProjectInfoDeltas::projectLicense));
    const String &licenseDelta = state.getProperty(Serialization::VCS::delta);
    if (licenseDelta != this->license)
    {
        this->setLicense(licenseDelta);
    }
}

void ProjectMetadata::resetFullNameDelta(const SerializedData &state)
{
    jassert(state.hasType(Serialization::VCS::ProjectInfoDeltas::projectTitle));
    const String &nameDelta = state.getProperty(Serialization::VCS::delta);
    if (nameDelta != this->getFullName())
    {
        this->setFullName(nameDelta);
    }
}

void ProjectMetadata::resetAuthorDelta(const SerializedData &state)
{
    jassert(state.hasType(Serialization::VCS::ProjectInfoDeltas::projectAuthor));
    const String &authorDelta = state.getProperty(Serialization::VCS::delta);
    if (authorDelta != this->author)
    {
        this->setAuthor(authorDelta);
    }
}

void ProjectMetadata::resetDescriptionDelta(const SerializedData &state)
{
    jassert(state.hasType(Serialization::VCS::ProjectInfoDeltas::projectDescription));
    const String &descriptionDelta = state.getProperty(Serialization::VCS::delta);
    if (descriptionDelta != this->description)
    {
        this->setDescription(descriptionDelta);
    }
}

void ProjectMetadata::resetTemperamentDelta(const SerializedData &state)
{
    jassert(state.hasType(Serialization::VCS::ProjectInfoDeltas::projectTemperament));
    if (state.getNumChildren() > 0)
    {
        jassert(this->temperament != nullptr);
        this->temperament->deserialize(state.getChild(0));
    }
}

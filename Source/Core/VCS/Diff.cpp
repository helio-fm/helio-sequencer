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
#include "Diff.h"
#include "DiffLogic.h"

using namespace VCS;

Diff::Diff(TrackedItem &diffTarget)
{
    this->description = diffTarget.getVCSName();
    this->vcsUuid = diffTarget.getUuid();
    this->logic = DiffLogic::createLogicCopy(diffTarget, *this);
}

Diff::~Diff()
{

}

bool Diff::hasAnyChanges() const
{
    return (this->getNumDeltas() > 0);
}

void Diff::addOwnedDelta(Delta *newDelta, XmlElement *newDeltaData)
{
    this->deltas.add(newDelta);
    this->deltasData.add(newDeltaData);
}

void Diff::clear()
{
    this->deltas.clear();
    this->deltasData.clear();
}


//===----------------------------------------------------------------------===//
// TrackedItem
//===----------------------------------------------------------------------===//

int Diff::getNumDeltas() const
{
    return this->deltas.size();
}

Delta *Diff::getDelta(int index) const
{
    return this->deltas[index];
}

XmlElement *Diff::createDeltaDataFor(int index) const
{
    const XmlElement *deltaData = this->deltasData[index];
    return new XmlElement(*deltaData);
}

String Diff::getVCSName() const
{
    return this->description;
}

DiffLogic *Diff::getDiffLogic() const
{
    return this->logic;
}

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
#include "ScaleClass.h"
#include "SerializationKeys.h"

using namespace Scripting;
using namespace Serialization::Scripts;

ScaleClass::ScaleClass(const Scale::Ptr scale) : scale(scale)
{
    this->setProperty(Api::Scale::name, scale->getLocalizedName());
    this->setProperty(Api::Scale::basePeriod, scale->getBasePeriod());

    this->setMethod(Api::Scale::hasKey, hasKey);
    this->setMethod(Api::Scale::getScaleKey, getScaleKey);
    this->setMethod(Api::Scale::getChromaticKey, getChromaticKey);
}

var ScaleClass::hasKey(Args &a)
{
    if (auto *self = dynamic_cast<ScaleClass *>(a.thisObject.getObject()))
    {
        if (a.numArguments == 1)
        {
            const int key = a.arguments[0];
            return var(self->scale->hasKey(key));
        }
    }

    return var(false);
}

var Scripting::ScaleClass::getScaleKey(Args &a)
{
    if (auto *self = dynamic_cast<ScaleClass *>(a.thisObject.getObject()))
    {
        if (a.numArguments == 1)
        {
            const int key = a.arguments[0];
            return var(self->scale->getScaleKey(key));
        }
    }

    return var(-1);
}

var Scripting::ScaleClass::getChromaticKey(Args &a)
{
    if (auto *self = dynamic_cast<ScaleClass *>(a.thisObject.getObject()))
    {
        if (a.numArguments == 1)
        {
            const int key = a.arguments[0];
            return var(self->scale->getChromaticKey(key));
        }
    }

    return var(-1);
}

/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#if !NO_NETWORK

#include "ApiModel.h"

class RevisionDto final : public ApiModel
{
public:

    RevisionDto() noexcept : ApiModel({}) {}
    RevisionDto(const SerializedData &tree) noexcept : ApiModel(tree) {}

    String getId() const noexcept { return DTO_PROPERTY(Revisions::id); }
    String getMessage() const noexcept { return DTO_PROPERTY(Revisions::message); }
    String getParentId() const noexcept { return DTO_PROPERTY(Revisions::parentId); }
    int64 getTimestamp() const noexcept { return DTO_PROPERTY(Revisions::timestamp); }
    SerializedData getData() const noexcept { return DTO_CHILD(Revisions::data); }

    JUCE_LEAK_DETECTOR(RevisionDto)
};

#endif

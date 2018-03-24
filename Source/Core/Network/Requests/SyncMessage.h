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

namespace VCS
{
    struct SyncMessage
    {
        SyncMessage() : result(SyncMessage::progress) {}

        enum Result
        {
            progress,
            error,
            success
        };

        void set(const String &text)
        {
            this->result = SyncMessage::progress;
            this->message = text;
        }

        void fail(const String &text)
        {
            this->status = text;
            this->result = SyncMessage::error;
        }

        void finish(const String &text)
        {
            this->status = text;
            this->result = SyncMessage::success;
        }

        String message;
        String status;
        Result result;
    };
}  // namespace VCS

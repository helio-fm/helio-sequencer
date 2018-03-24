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

class ClipboardOwner;

class InternalClipboard final
{
public:

    static void copy(const ClipboardOwner &owner, bool mirrorToSystemClipboard = false);
    static void paste(ClipboardOwner &owner);
    
    static ValueTree getCurrentContent();
    static String getCurrentContentAsString();

public:

    void copyFrom(const ClipboardOwner &owner, bool mirrorToSystemClipboard = false);
    void pasteTo(ClipboardOwner &owner);

private:

    ValueTree clipboard;

};

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

#include "TreeNode.h"

template <class TreeItemType>
class SafeTreeItemPointer
{
public:
    /** Creates a null SafeTreeItemPointer. */
    SafeTreeItemPointer() noexcept {}

    /** Creates a SafeTreeItemPointer that points at the given component. */
    explicit SafeTreeItemPointer (TreeItemType* const component)        : weakRef (component) {}

    /** Creates a copy of another SafeTreeItemPointer. */
    SafeTreeItemPointer (const SafeTreeItemPointer& other) noexcept     : weakRef (other.weakRef) {}

    /** Copies another pointer to this one. */
    SafeTreeItemPointer& operator= (const SafeTreeItemPointer& other)           { weakRef = other.weakRef; return *this; }

    /** Copies another pointer to this one. */
    SafeTreeItemPointer& operator= (TreeItemType* const newComponent)  { weakRef = newComponent; return *this; }

    /** Returns the component that this pointer refers to, or null if the component no longer exists. */
    TreeItemType* getComponent() const noexcept        { return dynamic_cast <TreeItemType*> (weakRef.get()); }

    /** Returns the component that this pointer refers to, or null if the component no longer exists. */
    operator TreeItemType*() const noexcept            { return getComponent(); }

    /** Returns the component that this pointer refers to, or null if the component no longer exists. */
    TreeItemType* operator->() noexcept                { return getComponent(); }

    /** Returns the component that this pointer refers to, or null if the component no longer exists. */
    const TreeItemType* operator->() const noexcept    { return getComponent(); }

    /** If the component is valid, this deletes it and sets this pointer to null. */
    void deleteAndZero()                                { delete getComponent(); jassert (getComponent() == nullptr); }

    bool operator== (TreeItemType* component) const noexcept   { return weakRef == component; }
    bool operator!= (TreeItemType* component) const noexcept   { return weakRef != component; }

private:
    WeakReference<TreeNode> weakRef;
};

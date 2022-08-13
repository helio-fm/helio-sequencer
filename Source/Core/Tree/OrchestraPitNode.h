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

class Instrument;
class InstrumentNode;

#include "TreeNode.h"
#include "OrchestraPitPage.h"
#include "OrchestraPit.h"

class OrchestraPitNode final : public TreeNode, private OrchestraListener
{
public:

    OrchestraPitNode();
    ~OrchestraPitNode() override;

    String getName() const noexcept override;
    Image getIcon() const noexcept override;

    void showPage() override;
    void recreatePage() override;

    //===------------------------------------------------------------------===//
    // Menu
    //===------------------------------------------------------------------===//

    bool hasMenu() const noexcept override;
    UniquePointer<Component> createMenu() override;

    //===------------------------------------------------------------------===//
    // OrchestraListener
    //===------------------------------------------------------------------===//

    void onAddInstrument(Instrument *instrument) override;
    void onRemoveInstrument(Instrument *instrument) override;
    void onPostRemoveInstrument() override {}

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;

private:
    
    OrchestraPit &orchestra;

    UniquePointer<OrchestraPitPage> instrumentsPage;

    void syncAllInstruments();

};

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

#include "Instrument.h"

class InstrumentEditorNode;
class InstrumentEditorConnector;
class InstrumentEditorPin;
class AudioCore;

class InstrumentEditor :
    public Component,
    public ChangeListener
{
public:

    enum ColourIds
    {
        midiInColourId       = 0x99004000,
        midiOutColourId      = 0x99004010,
        audioInColourId      = 0x99004020,
        audioOutColourId     = 0x99004030,
    };


    InstrumentEditor(Instrument &instrument,
                     WeakReference<AudioCore> audioCoreRef);

    ~InstrumentEditor() override;


    void updateComponents();


    InstrumentEditorNode *getComponentForFilter(const uint32 filterID) const;
    InstrumentEditorConnector *getComponentForConnection(const AudioProcessorGraph::Connection &conn) const;
    InstrumentEditorPin *findPinAt(const int x, const int y) const;

    void mouseDown(const MouseEvent &e) override;
    void resized() override;

    // Listens to instrument and audio device
    void changeListenerCallback(ChangeBroadcaster *) override;


    //===------------------------------------------------------------------===//
    // Dragging
    //===------------------------------------------------------------------===//

    void beginConnectorDrag(const uint32 sourceFilterID, const int sourceFilterChannel,
                            const uint32 destFilterID, const int destFilterChannel,
                            const MouseEvent &e);

    void dragConnector(const MouseEvent &e);

    void endDraggingConnector(const MouseEvent &e);

private:

    Instrument &instrument;

    ScopedPointer<Component> background;

    ScopedPointer<InstrumentEditorConnector> draggingConnector;

    WeakReference<AudioCore> audioCore;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentEditor)
};

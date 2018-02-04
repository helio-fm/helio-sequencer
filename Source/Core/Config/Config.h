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

class Serializable;

class Config :
    protected PropertySet,
    private Timer
{
public:

    explicit Config(const int millisecondsBeforeSaving = 3000);
    ~Config() override;

    static String getMachineId();
    static bool hasNewMachineId();

    static void set(const String &keyName, const var &value);
    static void set(const String &keyName, const XmlElement *xml);

    static String get(StringRef keyName, const String &defaultReturnValue = String::empty);
    static bool contains(StringRef keyName);
    static XmlElement *getXml(StringRef keyName);

    static void save(const String &key, const Serializable *serializer);
    static void load(const String &key, Serializable *serializer);

protected:

    void saveConfig(const String &key, const Serializable *serializer);
    bool saveIfNeeded();
    void loadConfig(const String &key, Serializable *serializer);
    bool reload();

    bool machineIdChanged();
    void propertyChanged() override;

private:

    void timerCallback() override;

    InterProcessLock fileLock;
    File propertiesFile;
    
    bool needsWriting;
    int saveTimeout;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Config)
};

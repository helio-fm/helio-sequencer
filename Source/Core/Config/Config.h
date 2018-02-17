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

class Config final : private Timer
{
public:

    explicit Config(int timeoutToSaveMs = 3000);
    ~Config() override;

    static String getDeviceId();

    static void set(const Identifier &key, const var &value);
    static String get(const Identifier &key, const String &fallback = {});
    static bool contains(const Identifier &key);

    static void save(const Identifier &key, const Serializable *serializer);
    static void load(const Identifier &key, Serializable *serializer);

private:

    void onConfigChanged();
    bool saveIfNeeded();
    bool reload();

    void saveConfigFor(const Identifier &key, const Serializable *serializer);
    void loadConfigFor(const Identifier &key, Serializable *serializer);
    void setProperty(const Identifier &key, const var &value);
    String getProperty(const Identifier &key, const String &fallback) const noexcept;
    bool containsPropertyOrChild(const Identifier &key) const noexcept;

private:

    void timerCallback() override;

    InterProcessLock fileLock;
    File propertiesFile;
    
    ValueTree config;

    bool needsSaving;
    int saveTimeout;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Config)
};

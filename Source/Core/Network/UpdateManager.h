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

#if HELIO_DESKTOP
#   undef UPDATE_MANAGER_IS_OFF
#elif HELIO_MOBILE
#   define UPDATE_MANAGER_IS_OFF
#endif

class DeferredUpdateDialogLauncher;

class UpdateManager :
    private Thread,
    public ChangeBroadcaster,
    private Timer
{
public:
    
    enum UpdateStatus
    {
        Unknown,
        UpToDate,
        RevisionChanges,
        MinorChanges,
        MajorChanges,
        UnableToConnect,
        ResponseParseError
    };
    
    UpdateManager();

    ~UpdateManager() override;
    
    bool hasUpdatePending() const;
    
    bool hasRevisionUpdate() const;
    bool hasMinorUpdate() const;
    bool hasMajorUpdate() const;
    
    String getLatestVersion() const;
    String getUpdateUrl() const;
    
protected:
    
    bool checkForUpdates(const String &url);
    void onUpdateThreadDone(UpdateStatus status,
                            const String &newVersion = "",
                            const String &url = "");

private:

    //===------------------------------------------------------------------===//
    // Thread
    //===------------------------------------------------------------------===//

    void run() override;
    
    //===------------------------------------------------------------------===//
    // Timer
    //===------------------------------------------------------------------===//
    
    void timerCallback() override;

    String checkUrl;

    ReadWriteLock statusLock;
    UpdateStatus updateStatus;
    String latestVersion;
    String updateUrl;
    
private:
    
    ScopedPointer<DeferredUpdateDialogLauncher> deferredDialogLauncher;
    
private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UpdateManager);

};

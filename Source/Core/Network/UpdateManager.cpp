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
#include "UpdateManager.h"
#include "Config.h"
#include "App.h"
#include "HelioServerDefines.h"
#include "DataEncoder.h"
#include "SerializationKeys.h"
#include "UpdateDialog.h"

#if JUCE_WINDOWS
#define APP_PLATFORM        ("HELIO_WINDOWS")
#endif

#if JUCE_MAC
#define APP_PLATFORM        ("HELIO_MAC")
#endif

#if JUCE_LINUX
#define APP_PLATFORM        ("HELIO_LINUX")
#endif

#if JUCE_32BIT
#define APP_BIT             ("HELIO_32BIT")
#endif

#if JUCE_64BIT
#define APP_BIT             ("HELIO_64BIT")
#endif

#define UPDATE_MANAGER_START_DELAY_MS (1000 * 10)
#define UPDATE_MANAGER_SHOWS_UPDATE_DIALOG 1

#define UPDATE_MANAGER_MINOR_UPDATE_TIMER_MS (1000 * 60 * 2)
#define UPDATE_MANAGER_MAJOR_UPDATE_TIMER_MS (1000 * 5)
#define UPDATE_MANAGER_MODAL_WAIT_TIMER_MS (1000 * 2)

class DeferredUpdateDialogLauncher : private Timer
{
public:

    explicit DeferredUpdateDialogLauncher(UpdateManager &parentManager) :
        updateManager(parentManager)
    {
    }
    
    void launchDialogInInterval(int timerIntervalMilliseconds)
    {
        Logger::writeToLog("DeferredUpdateDialogLauncher::launchDialogInInterval " + String(timerIntervalMilliseconds));
        this->startTimer(timerIntervalMilliseconds);
    }
    
    bool isWaitingToLaunch() const
    {
        return this->isTimerRunning();
    }
    
private:
    
    void timerCallback() override
    {
        if (! this->updateManager.hasUpdatePending())
        {
            this->stopTimer();
        }
        
        if (Component *modal = Component::getCurrentlyModalComponent(0))
        {
            Logger::writeToLog("DeferredUpdateDialogLauncher waits");
            this->stopTimer();
            this->startTimer(UPDATE_MANAGER_MODAL_WAIT_TIMER_MS);
        }
        else
        {
            this->showUpdateDialog();
            this->stopTimer();
        }
    }
    
    void showUpdateDialog()
    {
        if (! this->updateManager.hasUpdatePending())
        {
            return;
        }
        
        if (this->updateManager.hasMajorUpdate())
        {
            App::Helio()->showBlocker(new UpdateDialog());
        }
        else
        {
            App::Helio()->showModalComponent(new UpdateDialog());
        }
    }
    
private:
    
    UpdateManager &updateManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeferredUpdateDialogLauncher)
};


UpdateManager::UpdateManager() :
    Thread("Update thread"),
    updateStatus(UpdateManager::Unknown),
    latestVersion(""),
    updateUrl("")
{
    this->deferredDialogLauncher = new DeferredUpdateDialogLauncher(*this);
    
#if !defined UPDATE_MANAGER_IS_OFF
    this->startTimer(UPDATE_MANAGER_START_DELAY_MS);
#endif
}

UpdateManager::~UpdateManager()
{
    this->stopThread(100);
}


bool UpdateManager::hasUpdatePending() const
{
    const ScopedReadLock lock(this->statusLock);
    return (this->updateStatus == UpdateManager::MajorChanges ||
            this->updateStatus == UpdateManager::MinorChanges ||
            this->updateStatus == UpdateManager::RevisionChanges);
}

bool UpdateManager::hasRevisionUpdate() const
{
    const ScopedReadLock lock(this->statusLock);
    return (this->updateStatus == UpdateManager::RevisionChanges);
}

bool UpdateManager::hasMinorUpdate() const
{
    const ScopedReadLock lock(this->statusLock);
    return (this->updateStatus == UpdateManager::MinorChanges);
}

bool UpdateManager::hasMajorUpdate() const
{
    const ScopedReadLock lock(this->statusLock);
    return (this->updateStatus == UpdateManager::MajorChanges);
}


String UpdateManager::getLatestVersion() const
{
    const ScopedReadLock lock(this->statusLock);
    return this->latestVersion;
}

String UpdateManager::getUpdateUrl() const
{
    const ScopedReadLock lock(this->statusLock);
    return this->updateUrl;
}


bool UpdateManager::checkForUpdates(const String &url)
{
    if (this->isThreadRunning())
    {
        return false;
    }

    this->checkUrl = url;
    this->startThread(3);
    return true;
}

void UpdateManager::onUpdateThreadDone(UpdateStatus status,
                                       const String &newVersion,
                                       const String &url)
{
    const ScopedWriteLock lock(this->statusLock);
    this->updateStatus = status;
    this->latestVersion = newVersion;
    this->updateUrl = url;
}


//===----------------------------------------------------------------------===//
// Timer
//===----------------------------------------------------------------------===//

void UpdateManager::timerCallback()
{
    this->stopTimer();
    this->checkForUpdates(HELIO_UPDATES_CHECK_URL);
}


//===----------------------------------------------------------------------===//
// Thread
//===----------------------------------------------------------------------===//

void UpdateManager::run()
{
#if defined APP_PLATFORM
    const String deviceId(Config::getMachineId());
    const String saltedDeviceId = deviceId + HELIO_SALT;
    const String saltedDeviceIdHash = SHA256(saltedDeviceId.toUTF8()).toHexString();
    
    URL updateURL(checkUrl);
    updateURL = updateURL.withParameter(Serialization::Network::deviceId, deviceId);
    updateURL = updateURL.withParameter(Serialization::Network::clientCheck, saltedDeviceIdHash);
    updateURL = updateURL.withParameter(Serialization::Network::major, APP_VERSION_MAJOR);
    updateURL = updateURL.withParameter(Serialization::Network::minor, APP_VERSION_MINOR);
    updateURL = updateURL.withParameter(Serialization::Network::revision, APP_VERSION_REVISION);
    updateURL = updateURL.withParameter(Serialization::Network::platform, APP_PLATFORM);
    updateURL = updateURL.withParameter(Serialization::Network::bit, APP_BIT);

    int statusCode = 0;
    StringPairArray responseHeaders;
    ScopedPointer<InputStream> io(
                                  updateURL.createInputStream(true,
                                                              nullptr, nullptr,
                                                              HELIO_USERAGENT,
                                                              0, &responseHeaders, &statusCode));
    
    if (io == nullptr || statusCode != 200)
    {
        this->onUpdateThreadDone(UpdateManager::UnableToConnect);
        return;
    }
    
    const String entireStream = io->readEntireStreamAsString();
    //Logger::writeToLog(entireStream);
    
    var json;
    Result result = JSON::parse(entireStream, json);
    
    if (! result.wasOk())
    {
        Logger::writeToLog("JSON parse error: " + result.getErrorMessage());
        this->onUpdateThreadDone(UpdateManager::ResponseParseError);
        return;
    }

    int responseUpdateStatus = 0;
    String responseLatestVersion;
    String responseUpdateUrl;
    
    {
        const Identifier statusProperty(Serialization::Network::updateStatus);
        const Identifier versionProperty(Serialization::Network::latestVersion);
        const Identifier urlProperty(Serialization::Network::updateUrl);
        
        if (DynamicObject *obj = json.getDynamicObject())
        {
            NamedValueSet &props(obj->getProperties());
            for (int j = 0; j < props.size(); ++j)
            {
                const Identifier key(props.getName(j));
                var value(props[key]);
                
                if (key == statusProperty)
                {
                    responseUpdateStatus = value;
                }
                else if (key == versionProperty)
                {
                    responseLatestVersion = value;
                }
                else if (key == urlProperty)
                {
                    responseUpdateUrl = value;
                }
            }
        }
        
        Logger::writeToLog("Update status: " + String(responseUpdateStatus));
        Logger::writeToLog("Latest version: " + responseLatestVersion);
        Logger::writeToLog("Update url: " + responseUpdateUrl);
        
        switch (responseUpdateStatus)
        {
            case 1:
                this->onUpdateThreadDone(UpdateManager::RevisionChanges, responseLatestVersion, responseUpdateUrl);
                break;
                
            case 2:
                this->onUpdateThreadDone(UpdateManager::MinorChanges, responseLatestVersion, responseUpdateUrl);
                break;
                
            case 3:
                this->onUpdateThreadDone(UpdateManager::MajorChanges, responseLatestVersion, responseUpdateUrl);
                break;
                
            default: // 0 and else:
                this->onUpdateThreadDone(UpdateManager::UpToDate, responseLatestVersion, responseUpdateUrl);
                break;
        }
    }
    
    this->sendChangeMessage();
    
#if UPDATE_MANAGER_SHOWS_UPDATE_DIALOG
    if (! this->deferredDialogLauncher->isWaitingToLaunch())
    {
        if (this->hasMinorUpdate())
        {
            this->deferredDialogLauncher->launchDialogInInterval(UPDATE_MANAGER_MINOR_UPDATE_TIMER_MS);
        }
        else if (this->hasMajorUpdate())
        {
            this->deferredDialogLauncher->launchDialogInInterval(UPDATE_MANAGER_MAJOR_UPDATE_TIMER_MS);
        }
    }
#endif
    
#endif
}

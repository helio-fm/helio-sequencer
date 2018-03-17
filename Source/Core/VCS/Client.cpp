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
#include "Client.h"
#include "VersionControl.h"
#include "PushThread.h"
#include "PullThread.h"
#include "RemovalThread.h"
#include "App.h"
#include "ProgressTooltip.h"
#include "FailTooltip.h"
#include "SuccessTooltip.h"
#include "HelioApiRoutes.h"

using namespace VCS;

Client::Client(VersionControl &versionControl) :
    vcs(versionControl),
    lastPushState(SyncThread::readyToRock),
    lastPullState(SyncThread::readyToRock),
    lastRemovalState(SyncThread::readyToRock),
    pushThread(nullptr),
    pullThread(nullptr),
    removalThread(nullptr) {}

bool Client::push()
{
    //Logger::writeToLog(DocumentReader::obfuscate(HELIO_CLOUD_URL));

    if (this->isPushing()) { return false; }
    if (this->isPulling()) { return false; }
    if (this->isRemoving()) { return false; }

    const auto vcsNode(this->vcs.serialize());

    this->pushThread =
        new PushThread(URL(HelioFM::Api::V1::vcs),
                       this->vcs.getPublicId(),
                       this->vcs.getParentName(),
                       this->vcs.getKey(),
                       vcsNode);
    
    this->pushThread->startThread(5);

    this->lastPushState = SyncThread::readyToRock;
    this->lastPullState = SyncThread::readyToRock;
    this->lastRemovalState = SyncThread::readyToRock;
    
    this->pushThread->addChangeListener(this);

    return true;
}

bool Client::pull()
{
    if (this->isPushing()) { return false; }
    if (this->isPulling()) { return false; }
    if (this->isRemoving()) { return false; }

    const auto vcsNode(this->vcs.serialize());

    this->pullThread =
        new PullThread(URL(HelioFM::Api::V1::vcs),
                       this->vcs.getPublicId(),
                       this->vcs.getKey(),
                       vcsNode);

    this->pullThread->startThread(5);

    this->lastPushState = SyncThread::readyToRock;
    this->lastPullState = SyncThread::readyToRock;
    this->lastRemovalState = SyncThread::readyToRock;
    
    this->pullThread->addChangeListener(this);

    return true;
}

bool Client::remove()
{
    if (this->isPushing()) { return false; }
    if (this->isPulling()) { return false; }
    if (this->isRemoving()) { return false; }
    
    this->removalThread =
        new RemovalThread(URL(HelioFM::Api::V1::vcs),
                          this->vcs.getPublicId(),
                          this->vcs.getKey());
    
    this->removalThread->startThread(5);
    
    this->lastPushState = SyncThread::readyToRock;
    this->lastPullState = SyncThread::readyToRock;
    this->lastRemovalState = SyncThread::readyToRock;
    
    this->removalThread->addChangeListener(this);
    
    return true;
}


void Client::changeListenerCallback(ChangeBroadcaster *source)
{
    if (source == this->pushThread)
    {
        this->lastPushState = this->pushThread->getState();

        if (this->lastPushState == SyncThread::connect)
        {
            const String tooltip(TRANS("vcs::push::contacting::progress"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = new ProgressTooltip();
            App::Helio()->showModalComponent(this->progressBar);
        }
        else if (this->lastPushState == SyncThread::connectError)
        {
            this->deletePushThread();

            const String tooltip(TRANS("vcs::push::contacting::error"));
            App::Helio()->showTooltip(tooltip);

            this->progressBar = nullptr;
            App::Helio()->showModalComponent(new FailTooltip());
        }
        else if (this->lastPushState == SyncThread::fetchHistory)
        {
            const String tooltip(TRANS("vcs::push::fetching::progress"));
            App::Helio()->showTooltip(tooltip);
        }
        else if (this->lastPushState == SyncThread::fetchHistoryError)
        {
            this->deletePushThread();

            const String tooltip(TRANS("vcs::push::fetching::error"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = nullptr;
            App::Helio()->showModalComponent(new FailTooltip());
        }
        else if (this->lastPushState == SyncThread::merge)
        {
            // это сообщение слишком быстро проскакивает
            //const String tooltip(TRANS("Merging history trees.."));
            //App::Helio()->showTooltip(tooltip);
        }
        else if (this->lastPushState == SyncThread::upToDate)
        {
            this->deletePushThread();

            const String tooltip(TRANS("vcs::push::merging::uptodate"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = nullptr;
            App::Helio()->showModalComponent(new SuccessTooltip());
        }
        else if (this->lastPushState == SyncThread::mergeError)
        {
            this->deletePushThread();

            const String tooltip(TRANS("vcs::push::merging::error"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = nullptr;
            App::Helio()->showModalComponent(new FailTooltip());
        }
        else if (this->lastPushState == SyncThread::sync)
        {
            const String tooltip(TRANS("vcs::push::sync::progress"));
            App::Helio()->showTooltip(tooltip);
        }
        else if (this->lastPushState == SyncThread::unauthorizedError)
        {
            this->deletePushThread();
            
            const String tooltip(TRANS("vcs::push::sync::error::unauthorized"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = nullptr;
            App::Helio()->showModalComponent(new FailTooltip());
        }
        else if (this->lastPushState == SyncThread::forbiddenError)
        {
            this->deletePushThread();
            
            const String tooltip(TRANS("vcs::push::sync::error::forbidden"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = nullptr;
            App::Helio()->showModalComponent(new FailTooltip());
        }
        else if (this->lastPushState == SyncThread::syncError)
        {
            this->deletePushThread();

            const String tooltip(TRANS("vcs::push::sync::error"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = nullptr;
            App::Helio()->showModalComponent(new FailTooltip());
        }
        else if (this->lastPushState == SyncThread::allDone)
        {
            this->vcs.incrementVersion(); // sic!
            this->deletePushThread();

            const String tooltip(TRANS("vcs::push::done"));
            App::Helio()->showTooltip(tooltip);

            this->progressBar = nullptr;
            App::Helio()->showModalComponent(new SuccessTooltip());
        }
    }
    
    else if (source == this->pullThread)
    {
        this->lastPullState = this->pullThread->getState();

        if (this->lastPullState == SyncThread::connect)
        {
            const String tooltip(TRANS("vcs::pull::contacting::progress"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = new ProgressTooltip();
            App::Helio()->showModalComponent(this->progressBar);
        }
        else if (this->lastPullState == SyncThread::connectError)
        {
            this->deletePullThread();

            const String tooltip(TRANS("vcs::pull::contacting::error"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = nullptr;
            App::Helio()->showModalComponent(new FailTooltip());
        }
        else if (this->lastPullState == SyncThread::fetchHistory)
        {
            const String tooltip(TRANS("vcs::pull::fetching::progress"));
            App::Helio()->showTooltip(tooltip);
        }
        else if (this->lastPullState == SyncThread::fetchHistoryError)
        {
            this->deletePullThread();

            const String tooltip(TRANS("vcs::pull::fetching::error"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = nullptr;
            App::Helio()->showModalComponent(new FailTooltip());
        }
        else if (this->lastPullState == SyncThread::merge)
        {
            // это сообщение слишком быстро проскакивает
            //const String tooltip(TRANS("Merging history trees.."));
            //App::Helio()->showTooltip(tooltip);
        }
        else if (this->lastPullState == SyncThread::upToDate)
        {
            this->deletePullThread();

            const String tooltip(TRANS("vcs::pull::merging::uptodate"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = nullptr;
            App::Helio()->showModalComponent(new SuccessTooltip());
        }
        else if (this->lastPullState == SyncThread::mergeError)
        {
            this->deletePullThread();

            const String tooltip(TRANS("vcs::pull::merging::error"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = nullptr;
            App::Helio()->showModalComponent(new FailTooltip());
        }
        else if (this->lastPullState == SyncThread::allDone)
        {
            const auto mergedXml(this->pullThread->createMergedStateData());
            this->vcs.deserialize(mergedXml);
            this->vcs.checkout(this->vcs.getHead().getHeadingRevision());
            this->deletePullThread();
            
            const String tooltip(TRANS("vcs::pull::done"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = nullptr;
            App::Helio()->showModalComponent(new SuccessTooltip());
        }
    }
    
    else if (source == this->removalThread)
    {
        this->lastRemovalState = this->removalThread->getState();
        
        if (this->lastRemovalState == SyncThread::connect)
        {
            const String tooltip(TRANS("vcs::remove::contacting::progress"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = new ProgressTooltip();
            App::Helio()->showModalComponent(this->progressBar);
        }
        else if (this->lastRemovalState == SyncThread::connectError)
        {
            this->deleteRemovalThread();
            
            const String tooltip(TRANS("vcs::remove::contacting::error"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = nullptr;
            App::Helio()->showModalComponent(new FailTooltip());
        }
        else if (this->lastRemovalState == SyncThread::sync)
        {
            const String tooltip(TRANS("Purging history.."));
            App::Helio()->showTooltip(tooltip);
        }
        else if (this->lastRemovalState == SyncThread::syncError)
        {
            this->deleteRemovalThread();
            
            const String tooltip(TRANS("vcs::remove::sync::error"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = nullptr;
            App::Helio()->showModalComponent(new FailTooltip());
        }
        else if (this->lastRemovalState == SyncThread::unauthorizedError)
        {
            this->deleteRemovalThread();
            
            const String tooltip(TRANS("vcs::remove::sync::error::unauthorized"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = nullptr;
            App::Helio()->showModalComponent(new FailTooltip());
        }
        else if (this->lastRemovalState == SyncThread::forbiddenError)
        {
            this->deleteRemovalThread();
            
            const String tooltip(TRANS("vcs::remove::sync::error::forbidden"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = nullptr;
            App::Helio()->showModalComponent(new FailTooltip());
        }
        else if (this->lastRemovalState == SyncThread::notFoundError)
        {
            this->deleteRemovalThread();
            this->progressBar = nullptr;
            // don't show any messages here
        }
        else if (this->lastRemovalState == SyncThread::allDone)
        {
            this->deleteRemovalThread();
            
            const String tooltip(TRANS("vcs::remove::done"));
            App::Helio()->showTooltip(tooltip);
            
            this->progressBar = nullptr;
            App::Helio()->showModalComponent(new SuccessTooltip());
        }
    }

    this->sendChangeMessage();
}

void Client::deletePushThread()
{
    this->pushThread->removeChangeListener(this);
    //this->pushThread = nullptr; // will stop by itself, do not kill him by force
}

void Client::deletePullThread()
{
    this->pullThread->removeChangeListener(this);
    //this->pullThread = nullptr;
}

void Client::deleteRemovalThread()
{
    this->removalThread->removeChangeListener(this);
    //this->removalThread = nullptr;
}


bool Client::isPushDone() const
{
    return this->pushFinishedWithSuccess() || this->pushFinishedWithFail();
}

bool Client::pushFinishedWithSuccess() const
{
    return (this->lastPushState == SyncThread::upToDate) ||
           (this->lastPushState == SyncThread::allDone);
}

bool Client::pushFinishedWithFail() const
{
    return (this->lastPushState == SyncThread::connectError) ||
           (this->lastPushState == SyncThread::fetchHistoryError) ||
           (this->lastPushState == SyncThread::mergeError) ||
           (this->lastPushState == SyncThread::unauthorizedError) ||
           (this->lastPushState == SyncThread::forbiddenError) ||
           (this->lastPushState == SyncThread::syncError);
}


bool Client::isPullDone() const
{
    return this->pullFinishedWithSuccess() || this->pullFinishedWithFail();
}

bool Client::pullFinishedWithSuccess() const
{
    return (this->lastPullState == SyncThread::upToDate) ||
           (this->lastPullState == SyncThread::allDone);
}

bool Client::pullFinishedWithFail() const
{
    return (this->lastPullState == SyncThread::connectError) ||
           (this->lastPullState == SyncThread::fetchHistoryError) ||
           (this->lastPullState == SyncThread::mergeError);
}


bool Client::removalFinishedWithSuccess() const
{
    return (this->lastRemovalState == SyncThread::allDone) ||
           (this->lastRemovalState == SyncThread::notFoundError); // this is also counted as success in case of removal
}

bool Client::removalFinishedWithFail() const
{
    return (this->lastRemovalState == SyncThread::connectError) ||
           (this->lastRemovalState == SyncThread::unauthorizedError) ||
           (this->lastRemovalState == SyncThread::forbiddenError) ||
           (this->lastRemovalState == SyncThread::syncError);
}

bool Client::isRemovalDone() const
{
    return this->removalFinishedWithSuccess() || this->removalFinishedWithFail();
}


//===----------------------------------------------------------------------===//
bool Client::isPushing() const
{
    if (this->pushThread != nullptr)
    {
        return this->pushThread->isThreadRunning();
    }
    
    return false;
}

bool Client::isPulling() const
{
    if (this->pullThread != nullptr)
    {
        return this->pullThread->isThreadRunning();
    }
    
    return false;
}

bool Client::isRemoving() const
{
    if (this->removalThread != nullptr)
    {
        return this->removalThread->isThreadRunning();
    }
    
    return false;
}

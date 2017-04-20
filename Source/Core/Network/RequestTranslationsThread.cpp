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
#include "RequestTranslationsThread.h"
#include "Config.h"

#include "DataEncoder.h"
#include "HelioServerDefines.h"
#include "Supervisor.h"
#include "SerializationKeys.h"
#include "TranslationManager.h"


RequestTranslationsThread::RequestTranslationsThread() :
    Thread("RequestTranslations"),
    listener(nullptr)
{

}

RequestTranslationsThread::~RequestTranslationsThread()
{
    this->stopThread(100);
}

const String &RequestTranslationsThread::getLatestResponse()
{
	ScopedReadLock lock(this->dataLock);
	return this->latestResponse;
}


void RequestTranslationsThread::requestTranslations(RequestTranslationsThread::Listener *authListener)
{
    jassert(!this->isThreadRunning());
    this->listener = authListener;
    this->startThread(3);
}

void RequestTranslationsThread::run()
{
    URL fetchUrl(HELIO_TRANSLATIONS_URL);

    {
		int statusCode = 0;
		StringPairArray responseHeaders;

        ScopedPointer<InputStream> downloadStream(
			fetchUrl.createInputStream(true, nullptr, nullptr, HELIO_USERAGENT, 0, &responseHeaders, &statusCode));

		if (downloadStream != nullptr && statusCode == 200)
		{
			{
				ScopedWriteLock lock(this->dataLock);
				this->latestResponse = downloadStream->readEntireStreamAsString();
			}

			MessageManager::getInstance()->callFunctionOnMessageThread([](void *data) -> void*
				{
				RequestTranslationsThread *self = static_cast<RequestTranslationsThread *>(data);
					self->listener->translationsRequestOk();
					return nullptr;
				},
				this);
			
			return;
        }
    }

	MessageManager::getInstance()->callFunctionOnMessageThread([](void *data) -> void*
		{
			RequestTranslationsThread *self = static_cast<RequestTranslationsThread *>(data);
			self->listener->translationsRequestFailed();
			return nullptr;
		},
		this);
}

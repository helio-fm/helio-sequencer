#include <windows.h>
#include "asiosmpl.h"

static DWORD __stdcall ASIOThread (void *param);
static HANDLE ASIOThreadHandle = 0;
static bool done = false;
const double twoRaisedTo32 = 4294967296.;

AsioSample* theDriver = 0;

//------------------------------------------------------------------------------------------
void getNanoSeconds (ASIOTimeStamp* ts)
{
	double nanoSeconds = (double)((unsigned long)timeGetTime ()) * 1000000.;
	ts->hi = (unsigned long)(nanoSeconds / twoRaisedTo32);
	ts->lo = (unsigned long)(nanoSeconds - (ts->hi * twoRaisedTo32));
}

//------------------------------------------------------------------------------------------
void AsioSample::timerOn ()
{
	theDriver = this;
	DWORD asioId;
	done = false;
	ASIOThreadHandle = CreateThread (0, 0, &ASIOThread, 0, 0, &asioId);
}

//------------------------------------------------------------------------------------------
void AsioSample::timerOff ()
{
	done = true;
	if (ASIOThreadHandle)
		WaitForSingleObject(ASIOThreadHandle, 1000);
	ASIOThreadHandle = 0;
}

//------------------------------------------------------------------------------------------
static DWORD __stdcall ASIOThread (void *param)
{
	do
	{
		if (theDriver)
		{
			theDriver->bufferSwitch ();
			Sleep (theDriver->getMilliSeconds ());
		}
		else
		{
			double a = 1000. / 44100.;
			Sleep ((long)(a * (double)kBlockFrames));

		}
	} while (!done);
	return 0;
}

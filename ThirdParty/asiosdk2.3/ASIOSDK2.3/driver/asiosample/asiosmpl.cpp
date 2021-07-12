/*
	Steinberg Audio Stream I/O API
	(c) 1996, Steinberg Soft- und Hardware GmbH
	charlie (May 1996)

	asiosmpl.cpp
	
	sample implementation of asio. can be set to simulate input with some
	stupid oscillators.
	this driver doesn't output sound at all...
	timing is done via the extended time manager on the mac, and
	a simple thread on pc.
	you may test various configurations by changing the kNumInputs/Outputs,
	and kBlockFrames. note that when using wave generation, as i/o is not optimized
	at all, it moves quite a bit of memory, too many i/o channels may make it
	pretty slow.

	if you use this file as a template, make sure to resolve places where the
	search string !!! can be found...
*/

#include <stdio.h>
#include <string.h>
#include "asiosmpl.h"
//#include "virtape.h"

//------------------------------------------------------------------------------------------

// extern
void getNanoSeconds(ASIOTimeStamp *time);

// local

double AsioSamples2double (ASIOSamples* samples);

static const double twoRaisedTo32 = 4294967296.;
static const double twoRaisedTo32Reciprocal = 1. / twoRaisedTo32;

//------------------------------------------------------------------------------------------
// on windows, we do the COM stuff.

#if WINDOWS
#include "windows.h"
#include "mmsystem.h"

// class id. !!! NOTE: !!! you will obviously have to create your own class id!
// {188135E1-D565-11d2-854F-00A0C99F5D19}
CLSID IID_ASIO_DRIVER = { 0x188135e1, 0xd565, 0x11d2, { 0x85, 0x4f, 0x0, 0xa0, 0xc9, 0x9f, 0x5d, 0x19 } };

CFactoryTemplate g_Templates[1] = {
    {L"ASIOSAMPLE", &IID_ASIO_DRIVER, AsioSample::CreateInstance} 
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

CUnknown* AsioSample::CreateInstance (LPUNKNOWN pUnk, HRESULT *phr)
{
	return (CUnknown*)new AsioSample (pUnk,phr);
};

STDMETHODIMP AsioSample::NonDelegatingQueryInterface (REFIID riid, void ** ppv)
{
	if (riid == IID_ASIO_DRIVER)
	{
		return GetInterface (this, ppv);
	}
	return CUnknown::NonDelegatingQueryInterface (riid, ppv);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//		Register ASIO Driver
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
extern LONG RegisterAsioDriver (CLSID,char *,char *,char *,char *);
extern LONG UnregisterAsioDriver (CLSID,char *,char *);

//
// Server registration, called on REGSVR32.EXE "the dllname.dll"
//
HRESULT _stdcall DllRegisterServer()
{
	LONG	rc;
	char	errstr[128];

	rc = RegisterAsioDriver (IID_ASIO_DRIVER,"ASIOSample.dll","ASIO Sample Driver","ASIO Sample","Apartment");

	if (rc) {
		memset(errstr,0,128);
		sprintf(errstr,"Register Server failed ! (%d)",rc);
		MessageBox(0,(LPCTSTR)errstr,(LPCTSTR)"ASIO sample Driver",MB_OK);
		return -1;
	}

	return S_OK;
}

//
// Server unregistration
//
HRESULT _stdcall DllUnregisterServer()
{
	LONG	rc;
	char	errstr[128];

	rc = UnregisterAsioDriver (IID_ASIO_DRIVER,"ASIOSample.dll","ASIO Sample Driver");

	if (rc) {
		memset(errstr,0,128);
		sprintf(errstr,"Unregister Server failed ! (%d)",rc);
		MessageBox(0,(LPCTSTR)errstr,(LPCTSTR)"ASIO Korg1212 I/O Driver",MB_OK);
		return -1;
	}

	return S_OK;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
AsioSample::AsioSample (LPUNKNOWN pUnk, HRESULT *phr)
	: CUnknown("ASIOSAMPLE", pUnk, phr)

//------------------------------------------------------------------------------------------

#else

// when not on windows, we derive from AsioDriver
AsioSample::AsioSample () : AsioDriver ()

#endif
{
	long i;

	blockFrames = kBlockFrames;
	inputLatency = blockFrames;		// typically
	outputLatency = blockFrames * 2;
	// typically blockFrames * 2; try to get 1 by offering direct buffer
	// access, and using asioPostOutput for lower latency
	samplePosition = 0;
	sampleRate = 44100.;
	milliSeconds = (long)((double)(kBlockFrames * 1000) / sampleRate);
	active = false;
	started = false;
	timeInfoMode = false;
	tcRead = false;
	for (i = 0; i < kNumInputs; i++)
	{
		inputBuffers[i] = 0;
		inMap[i] = 0;
	}
#if TESTWAVES
	sawTooth = sineWave = 0;
#endif
	for (i = 0; i < kNumOutputs; i++)
	{
		outputBuffers[i] = 0;
		outMap[i] = 0;
	}
	callbacks = 0;
	activeInputs = activeOutputs = 0;
	toggle = 0;
}

//------------------------------------------------------------------------------------------
AsioSample::~AsioSample ()
{
	stop ();
	outputClose ();
	inputClose ();
	disposeBuffers ();
}

//------------------------------------------------------------------------------------------
void AsioSample::getDriverName (char *name)
{
	strcpy (name, "Sample ASIO");
}

//------------------------------------------------------------------------------------------
long AsioSample::getDriverVersion ()
{
	return 0x00000001L;
}

//------------------------------------------------------------------------------------------
void AsioSample::getErrorMessage (char *string)
{
	strcpy (string, errorMessage);
}

//------------------------------------------------------------------------------------------
ASIOBool AsioSample::init (void* sysRef)
{
	sysRef = sysRef;
	if (active)
		return true;
	strcpy (errorMessage, "ASIO Driver open Failure!");
	if (inputOpen ())
	{
		if (outputOpen ())
		{
			active = true;
			return true;
		}
	}
	timerOff ();		// de-activate 'hardware'

	outputClose ();
	inputClose ();
	return false;
}

//------------------------------------------------------------------------------------------
ASIOError AsioSample::start ()
{
	if (callbacks)
	{
		started = false;
		samplePosition = 0;
		theSystemTime.lo = theSystemTime.hi = 0;
		toggle = 0;

		timerOn ();			// activate 'hardware'
		started = true;

		return ASE_OK;
	}
	return ASE_NotPresent;
}

//------------------------------------------------------------------------------------------
ASIOError AsioSample::stop ()
{
	started = false;
	timerOff ();		// de-activate 'hardware'
	return ASE_OK;
}

//------------------------------------------------------------------------------------------
ASIOError AsioSample::getChannels (long *numInputChannels, long *numOutputChannels)
{
	*numInputChannels = kNumInputs;
	*numOutputChannels = kNumOutputs;
	return ASE_OK;
}

//------------------------------------------------------------------------------------------
ASIOError AsioSample::getLatencies (long *_inputLatency, long *_outputLatency)
{
	*_inputLatency = inputLatency;
	*_outputLatency = outputLatency;
	return ASE_OK;
}

//------------------------------------------------------------------------------------------
ASIOError AsioSample::getBufferSize (long *minSize, long *maxSize,
	long *preferredSize, long *granularity)
{
	*minSize = *maxSize = *preferredSize = blockFrames;		// allow this size only
	*granularity = 0;
	return ASE_OK;
}

//------------------------------------------------------------------------------------------
ASIOError AsioSample::canSampleRate (ASIOSampleRate sampleRate)
{
	if (sampleRate == 44100. || sampleRate == 48000.)		// allow these rates only
		return ASE_OK;
	return ASE_NoClock;
}

//------------------------------------------------------------------------------------------
ASIOError AsioSample::getSampleRate (ASIOSampleRate *sampleRate)
{
	*sampleRate = this->sampleRate;
	return ASE_OK;
}

//------------------------------------------------------------------------------------------
ASIOError AsioSample::setSampleRate (ASIOSampleRate sampleRate)
{
	if (sampleRate != 44100. && sampleRate != 48000.)
		return ASE_NoClock;
	if (sampleRate != this->sampleRate)
	{
		this->sampleRate = sampleRate;
		asioTime.timeInfo.sampleRate = sampleRate;
		asioTime.timeInfo.flags |= kSampleRateChanged;
		milliSeconds = (long)((double)(kBlockFrames * 1000) / this->sampleRate);
		if (callbacks && callbacks->sampleRateDidChange)
			callbacks->sampleRateDidChange (this->sampleRate);
	}
	return ASE_OK;
}

//------------------------------------------------------------------------------------------
ASIOError AsioSample::getClockSources (ASIOClockSource *clocks, long *numSources)
{
	// internal
	clocks->index = 0;
	clocks->associatedChannel = -1;
	clocks->associatedGroup = -1;
	clocks->isCurrentSource = ASIOTrue;
	strcpy(clocks->name, "Internal");
	*numSources = 1;
	return ASE_OK;
}

//------------------------------------------------------------------------------------------
ASIOError AsioSample::setClockSource (long index)
{
	if (!index)
	{
		asioTime.timeInfo.flags |= kClockSourceChanged;
		return ASE_OK;
	}
	return ASE_NotPresent;
}

//------------------------------------------------------------------------------------------
ASIOError AsioSample::getSamplePosition (ASIOSamples *sPos, ASIOTimeStamp *tStamp)
{
	tStamp->lo = theSystemTime.lo;
	tStamp->hi = theSystemTime.hi;
	if (samplePosition >= twoRaisedTo32)
	{
		sPos->hi = (unsigned long)(samplePosition * twoRaisedTo32Reciprocal);
		sPos->lo = (unsigned long)(samplePosition - (sPos->hi * twoRaisedTo32));
	}
	else
	{
		sPos->hi = 0;
		sPos->lo = (unsigned long)samplePosition;
	}
	return ASE_OK;
}

//------------------------------------------------------------------------------------------
ASIOError AsioSample::getChannelInfo (ASIOChannelInfo *info)
{
	if (info->channel < 0 || (info->isInput ? info->channel >= kNumInputs : info->channel >= kNumOutputs))
		return ASE_InvalidParameter;
#if WINDOWS
	info->type = ASIOSTInt16LSB;
#else
	info->type = ASIOSTInt16MSB;
#endif
	info->channelGroup = 0;
	info->isActive = ASIOFalse;
	long i;
	if (info->isInput)
	{
		for (i = 0; i < activeInputs; i++)
		{
			if (inMap[i] == info->channel)
			{
				info->isActive = ASIOTrue;
				break;
			}
		}
	}
	else
	{
		for (i = 0; i < activeOutputs; i++)
		{
			if (outMap[i] == info->channel)
			{
				info->isActive = ASIOTrue;
				break;
			}
		}
	}
	strcpy(info->name, "Sample ");
	return ASE_OK;
}

//------------------------------------------------------------------------------------------
ASIOError AsioSample::createBuffers (ASIOBufferInfo *bufferInfos, long numChannels,
	long bufferSize, ASIOCallbacks *callbacks)
{
	ASIOBufferInfo *info = bufferInfos;
	long i;
	bool notEnoughMem = false;

	activeInputs = 0;
	activeOutputs = 0;
	blockFrames = bufferSize;
	for (i = 0; i < numChannels; i++, info++)
	{
		if (info->isInput)
		{
			if (info->channelNum < 0 || info->channelNum >= kNumInputs)
				goto error;
			inMap[activeInputs] = info->channelNum;
			inputBuffers[activeInputs] = new short[blockFrames * 2];	// double buffer
			if (inputBuffers[activeInputs])
			{
				info->buffers[0] = inputBuffers[activeInputs];
				info->buffers[1] = inputBuffers[activeInputs] + blockFrames;
			}
			else
			{
				info->buffers[0] = info->buffers[1] = 0;
				notEnoughMem = true;
			}
			activeInputs++;
			if (activeInputs > kNumInputs)
			{
error:
				disposeBuffers();
				return ASE_InvalidParameter;
			}
		}
		else	// output			
		{
			if (info->channelNum < 0 || info->channelNum >= kNumOutputs)
				goto error;
			outMap[activeOutputs] = info->channelNum;
			outputBuffers[activeOutputs] = new short[blockFrames * 2];	// double buffer
			if (outputBuffers[activeOutputs])
			{
				info->buffers[0] = outputBuffers[activeOutputs];
				info->buffers[1] = outputBuffers[activeOutputs] + blockFrames;
			}
			else
			{
				info->buffers[0] = info->buffers[1] = 0;
				notEnoughMem = true;
			}
			activeOutputs++;
			if (activeOutputs > kNumOutputs)
			{
				activeOutputs--;
				disposeBuffers();
				return ASE_InvalidParameter;
			}
		}
	}		
	if (notEnoughMem)
	{
		disposeBuffers();
		return ASE_NoMemory;
	}

	this->callbacks = callbacks;
	if (callbacks->asioMessage (kAsioSupportsTimeInfo, 0, 0, 0))
	{
		timeInfoMode = true;
		asioTime.timeInfo.speed = 1.;
		asioTime.timeInfo.systemTime.hi = asioTime.timeInfo.systemTime.lo = 0;
		asioTime.timeInfo.samplePosition.hi = asioTime.timeInfo.samplePosition.lo = 0;
		asioTime.timeInfo.sampleRate = sampleRate;
		asioTime.timeInfo.flags = kSystemTimeValid | kSamplePositionValid | kSampleRateValid;

		asioTime.timeCode.speed = 1.;
		asioTime.timeCode.timeCodeSamples.lo = asioTime.timeCode.timeCodeSamples.hi = 0;
		asioTime.timeCode.flags = kTcValid | kTcRunning ;
	}
	else
		timeInfoMode = false;	
	return ASE_OK;
}

//---------------------------------------------------------------------------------------------
ASIOError AsioSample::disposeBuffers()
{
	long i;
	
	callbacks = 0;
	stop();
	for (i = 0; i < activeInputs; i++)
		delete inputBuffers[i];
	activeInputs = 0;
	for (i = 0; i < activeOutputs; i++)
		delete outputBuffers[i];
	activeOutputs = 0;
	return ASE_OK;
}

//---------------------------------------------------------------------------------------------
ASIOError AsioSample::controlPanel()
{
	return ASE_NotPresent;
}

//---------------------------------------------------------------------------------------------
ASIOError AsioSample::future (long selector, void* opt)	// !!! check properties 
{
	ASIOTransportParameters* tp = (ASIOTransportParameters*)opt;
	switch (selector)
	{
		case kAsioEnableTimeCodeRead:	tcRead = true;	return ASE_SUCCESS;
		case kAsioDisableTimeCodeRead:	tcRead = false;	return ASE_SUCCESS;
		case kAsioSetInputMonitor:		return ASE_SUCCESS;	// for testing!!!
		case kAsioCanInputMonitor:		return ASE_SUCCESS;	// for testing!!!
		case kAsioCanTimeInfo:			return ASE_SUCCESS;
		case kAsioCanTimeCode:			return ASE_SUCCESS;
	}
	return ASE_NotPresent;
}

//--------------------------------------------------------------------------------------------------------
// private methods
//--------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------
// input
//--------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
bool AsioSample::inputOpen ()
{
#if TESTWAVES
	sineWave = new short[blockFrames];
	if (!sineWave)
	{
		strcpy (errorMessage, "ASIO Sample Driver: Out of Memory!");
		return false;
	}
	makeSine (sineWave);

	sawTooth = new short[blockFrames];
	if (!sawTooth)
	{
		strcpy(errorMessage, "ASIO Sample Driver: Out of Memory!");
		return false;
	}
	makeSaw(sawTooth);
#endif
	return true;
}

#if TESTWAVES

#include <math.h>

const double pi = 0.3141592654;

//---------------------------------------------------------------------------------------------
void AsioSample::makeSine (short *wave)
{
	double frames = (double)blockFrames;
	double i, f = (pi * 2.) / frames;

	for (i = 0; i < frames; i++)
		*wave++ = (short)((double)0x7fff * sin(f * i));
}

//---------------------------------------------------------------------------------------------
void AsioSample::makeSaw(short *wave)
{
	double frames = (double)blockFrames;
	double i, f = 2. / frames;

	for (i = 0; i < frames; i++)
		*wave++ = (short)((double)0x7fff * (-1. + f * i));
}
#endif

//---------------------------------------------------------------------------------------------
void AsioSample::inputClose ()
{
#if TESTWAVES
	if (sineWave)
		delete sineWave;
	sineWave = 0;
	if (sawTooth)
		delete sawTooth;
	sawTooth = 0;
#endif
}

//---------------------------------------------------------------------------------------------
void AsioSample::input()
{
#if TESTWAVES
	long i;
	short *in = 0;

	for (i = 0; i < activeInputs; i++)
	{
		in = inputBuffers[i];
		if (in)
		{
			if (toggle)
				in += blockFrames;
			if ((i & 1) && sawTooth)
				memcpy(in, sawTooth, (unsigned long)(blockFrames * 2));
			else if (sineWave)
				memcpy(in, sineWave, (unsigned long)(blockFrames * 2));
		}
	}
#endif
}

//------------------------------------------------------------------------------------------------------------------
// output
//------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
bool AsioSample::outputOpen()
{
	return true;
}

//---------------------------------------------------------------------------------------------
void AsioSample::outputClose ()
{
}

//---------------------------------------------------------------------------------------------
void AsioSample::output ()
{
}

//---------------------------------------------------------------------------------------------
void AsioSample::bufferSwitch ()
{
	if (started && callbacks)
	{
		getNanoSeconds(&theSystemTime);			// latch system time
		input();
		output();
		samplePosition += blockFrames;
		if (timeInfoMode)
			bufferSwitchX ();
		else
			callbacks->bufferSwitch (toggle, ASIOFalse);
		toggle = toggle ? 0 : 1;
	}
}

//---------------------------------------------------------------------------------------------
// asio2 buffer switch
void AsioSample::bufferSwitchX ()
{
	getSamplePosition (&asioTime.timeInfo.samplePosition, &asioTime.timeInfo.systemTime);
	long offset = toggle ? blockFrames : 0;
	if (tcRead)
	{	// Create a fake time code, which is 10 minutes ahead of the card's sample position
		// Please note that for simplicity here time code will wrap after 32 bit are reached
		asioTime.timeCode.timeCodeSamples.lo = asioTime.timeInfo.samplePosition.lo + 600.0 * sampleRate;
		asioTime.timeCode.timeCodeSamples.hi = 0;
	}
	callbacks->bufferSwitchTimeInfo (&asioTime, toggle, ASIOFalse);
	asioTime.timeInfo.flags &= ~(kSampleRateChanged | kClockSourceChanged);
}

//---------------------------------------------------------------------------------------------
ASIOError AsioSample::outputReady ()
{
	return ASE_NotPresent;
}

//---------------------------------------------------------------------------------------------
double AsioSamples2double (ASIOSamples* samples)
{
	double a = (double)(samples->lo);
	if (samples->hi)
		a += (double)(samples->hi) * twoRaisedTo32;
	return a;
}


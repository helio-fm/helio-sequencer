/*
	Steinberg Audio Stream I/O API
	(c) 1996, Steinberg Soft- und Hardware GmbH
	charlie (May 1996)

	asiosmpl.cpp
	
	sample implementation of asio.
	time manager irq
	**** mac os specific ****
*/

#include <macheaders.h>
#include <Timer.h>
#include <string.h>
#include "asiosmpl.h"

static TMTask myTmTask;
static bool tmTaskOn = false;
static AsioSample *theSound = 0;

void myTimer();
void myTimer()
{
	if(theSound)
		theSound->bufferSwitch();
	PrimeTime((QElemPtr)&myTmTask, theSound->milliSeconds);
}

void AsioSample::timerOn()
{
	theSound = this;	// for irq
	if(!tmTaskOn)
	{
		memset(&myTmTask, 0, sizeof(TMTask));
		myTmTask.tmAddr = NewTimerProc(myTimer);
		myTmTask.tmWakeUp = 0;
		tmTaskOn = true;
		InsXTime((QElemPtr)&myTmTask);
		PrimeTime((QElemPtr)&myTmTask, theSound->milliSeconds);
	}
}

void AsioSample::timerOff()
{
	if(tmTaskOn)
	{
		RmvTime((QElemPtr)&myTmTask);
		tmTaskOn = false;
	}
	theSound = 0;
}

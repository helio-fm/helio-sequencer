#include "asio.h"
#include <Timer.h>

void getNanoSeconds(ASIOTimeStamp *time);

static const double twoRaisedTo32 = 4294967296.;

void getNanoSeconds(ASIOTimeStamp *time)
{
	UnsignedWide ys;
	Microseconds(&ys);
	double r = 1000. * ((double)ys.hi * twoRaisedTo32 + (double)ys.lo);
	time->hi = (unsigned long)(r / twoRaisedTo32);
	time->lo = (unsigned long)(r - (double)time->hi * twoRaisedTo32); 
}

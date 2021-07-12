#include "asiosmpl.h"

AsioDriver *getDriver()
{
	return new AsioSample();
}

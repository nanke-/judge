#include "a.h"
#include "logger.h"
extern "C"
{
	#include "logger.h"
}
void A::print(void)
{
	FM_LOG_DEBUG("print");
}

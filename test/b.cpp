#include "b.h"
#include "logger.h"
extern "C"
{
	#include "logger.h"
}
void B::print(void)
{
	FM_LOG_DEBUG("print");
}

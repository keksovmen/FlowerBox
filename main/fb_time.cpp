#include "fb_time.hpp"

#include <time.h>



using namespace fb;
using namespace clock;



Timestamp clock::currentTime()
{
	time_t now;
	time(&now);

	return now;
}

#pragma once



#include "fb_clock.hpp"



namespace fb
{
	namespace settings
	{
		//light switch
		void setLightStartTime(clock::Timestamp t);
		void setLightEndTime(clock::Timestamp t);

		//light switch
		clock::Timestamp getLightStartTime();
		clock::Timestamp getLightEndTime();
	}
}
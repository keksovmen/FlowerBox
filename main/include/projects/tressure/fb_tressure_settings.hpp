#pragma once



#include "fb_clock.hpp"



namespace fb {
	namespace settings {

		/* ------- setters ------- */
		
		void setPulseDuration(int value);
		
		void setBoostDelay(int value);
		
		void setMqttId(int value);
		

		/*  ------- getters ------- */
		
		int getPulseDuration();
		
		int getBoostDelay();
		
		int getMqttId();
		

	} // namespace settings
} // namespace fb
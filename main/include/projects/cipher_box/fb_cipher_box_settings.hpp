#pragma once



#include <string>

#include "fb_clock.hpp"



namespace fb {
	namespace settings {

		/* ------- setters ------- */
		
		void setUrl(std::string value);
		
		void setDeltaError(int value);
		
		void setBounceTimeMs(int value);
		

		/*  ------- getters ------- */
		
		std::string getUrl();
		
		int getDeltaError();
		
		int getBounceTimeMs();
		

	} // namespace settings
} // namespace fb
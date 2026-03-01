#pragma once



#include <string>

#include "fb_clock.hpp"



namespace fb {
	namespace settings {

		/* ------- setters ------- */
		
		void setUrl(std::string value);
		
		void setDeltaError(int value);
		
		void setBounceTimeMs(int value);
		
		void setChipsCount(int value);
		

		/*  ------- getters ------- */
		
		std::string getUrl();
		
		int getDeltaError();
		
		int getBounceTimeMs();
		
		int getChipsCount();
		

	} // namespace settings
} // namespace fb
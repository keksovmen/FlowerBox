#pragma once



#include <string>

#include "fb_clock.hpp"



namespace fb {
	namespace settings {

		/* ------- setters ------- */
		
		void setUrl(std::string value);
		
		void setBounceTimeMs(int value);
		

		/*  ------- getters ------- */
		
		std::string getUrl();
		
		int getBounceTimeMs();
		

	} // namespace settings
} // namespace fb
#pragma once



#include <string>

#include "fb_clock.hpp"



namespace fb {
	namespace settings {

		/* ------- setters ------- */
		
		void setHttpUrl(std::string value);
		
		void setHttpDelay(int value);
		

		/*  ------- getters ------- */
		
		std::string getHttpUrl();
		
		int getHttpDelay();
		

	} // namespace settings
} // namespace fb
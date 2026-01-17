#pragma once



#include <string>

#include "fb_clock.hpp"



namespace fb {
	namespace settings {

		/* ------- setters ------- */
		
		void setUrl(std::string value);
		
		void setDeltaError(int value);
		

		/*  ------- getters ------- */
		
		std::string getUrl();
		
		int getDeltaError();
		

	} // namespace settings
} // namespace fb
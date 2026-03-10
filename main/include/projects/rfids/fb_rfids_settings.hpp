#pragma once



#include <string>

#include "fb_clock.hpp"



namespace fb {
	namespace settings {

		/* ------- setters ------- */
		
		void setUrl(std::string value);
		
		void setLog(int value);
		
		void setReadTimeMs(int value);
		

		/*  ------- getters ------- */
		
		std::string getUrl();
		
		int getLog();
		
		int getReadTimeMs();
		

	} // namespace settings
} // namespace fb
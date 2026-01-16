#pragma once



#include <string>

#include "fb_clock.hpp"



namespace fb {
	namespace settings {

		/* ------- setters ------- */
		
		void setMqttId(int value);
		
		void setIp(std::string value);
		
		void setPort(int value);
		

		/*  ------- getters ------- */
		
		int getMqttId();
		
		std::string getIp();
		
		int getPort();
		

	} // namespace settings
} // namespace fb
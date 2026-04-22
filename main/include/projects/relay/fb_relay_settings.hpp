#pragma once



#include <string>

#include "fb_clock.hpp"



namespace fb {
	namespace settings {

		/* ------- setters ------- */
		
		void setHttpUrl(std::string value);
		
		void setHttpDelay(int value);
		
		void setMqttId(int value);
		
		void setIp(std::string value);
		
		void setPort(int value);
		

		/*  ------- getters ------- */
		
		std::string getHttpUrl();
		
		int getHttpDelay();
		
		int getMqttId();
		
		std::string getIp();
		
		int getPort();
		

	} // namespace settings
} // namespace fb
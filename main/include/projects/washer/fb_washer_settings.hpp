#pragma once



#include <string>

#include "fb_clock.hpp"



namespace fb {
	namespace settings {

		/* ------- setters ------- */
		
		void setUrl(std::string value);
		
		void setBounceTimeMs(int value);
		
		void setMqttId(int value);
		
		void setIp(std::string value);
		
		void setPort(int value);
		
		void setBlinkPeriodMs(int value);
		

		/*  ------- getters ------- */
		
		std::string getUrl();
		
		int getBounceTimeMs();
		
		int getMqttId();
		
		std::string getIp();
		
		int getPort();
		
		int getBlinkPeriodMs();
		

	} // namespace settings
} // namespace fb
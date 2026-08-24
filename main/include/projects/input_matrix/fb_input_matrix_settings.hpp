#pragma once



#include <string>

#include "fb_clock.hpp"



namespace fb {
	namespace settings {

		/* ------- setters ------- */
		
		void setMqttId(int value);
		
		void setIp(std::string value);
		
		void setPort(int value);
		
		void setPulseTime(int value);
		
		void setCombination(std::string value);
		
		void setDebounceMs(int value);
		
		void setStillPeriodMs(int value);
		
		void setEnableTarget(int value);
		

		/*  ------- getters ------- */
		
		int getMqttId();
		
		std::string getIp();
		
		int getPort();
		
		int getPulseTime();
		
		std::string getCombination();
		
		int getDebounceMs();
		
		int getStillPeriodMs();
		
		int getEnableTarget();
		

	} // namespace settings
} // namespace fb
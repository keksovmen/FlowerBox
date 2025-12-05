#pragma once



#include <string>

#include "fb_clock.hpp"



namespace fb {
	namespace settings {

		/* ------- setters ------- */
		
		void setPulseDuration(int value);
		
		void setBoostDelay(int value);
		
		void setMqttId(int value);
		
		void setBatteryMax(int value);
		
		void setBatteryMin(int value);
		
		void setIp(std::string value);
		
		void setPort(int value);
		
		void setLedRed(int value);
		
		void setLedGreen(int value);
		
		void setLedBlue(int value);
		

		/*  ------- getters ------- */
		
		int getPulseDuration();
		
		int getBoostDelay();
		
		int getMqttId();
		
		int getBatteryMax();
		
		int getBatteryMin();
		
		std::string getIp();
		
		int getPort();
		
		int getLedRed();
		
		int getLedGreen();
		
		int getLedBlue();
		

	} // namespace settings
} // namespace fb
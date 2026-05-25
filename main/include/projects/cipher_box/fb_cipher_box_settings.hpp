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
		
		void setReadingsCount(int value);
		
		void setMqttId(int value);
		
		void setIp(std::string value);
		
		void setPort(int value);
		
		void setUseHttp(int value);
		

		/*  ------- getters ------- */
		
		std::string getUrl();
		
		int getDeltaError();
		
		int getBounceTimeMs();
		
		int getChipsCount();
		
		int getReadingsCount();
		
		int getMqttId();
		
		std::string getIp();
		
		int getPort();
		
		int getUseHttp();
		

	} // namespace settings
} // namespace fb
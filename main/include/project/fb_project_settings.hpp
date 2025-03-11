#pragma once



#include "fb_clock.hpp"



namespace fb
{
	namespace settings
	{
		//setters
		//light switch
		void setLightStartTime(clock::Timestamp t);
		void setLightEndTime(clock::Timestamp t);

		//fan switch
		void setFanSpeed(int speed);
		void setFanLowTemp(float value);
		void setFanHighTemp(float value);

		//heater switch
		void setHeaterLowTemp(float value);
		void setHeaterHighTemp(float value);


		//getters
		//light switch
		clock::Timestamp getLightStartTime();
		clock::Timestamp getLightEndTime();

		//fan switch
		int getFanSpeed();
		float getFanLowTemp();
		float getFanHighTemp();

		//heater switch
		float getHeaterLowTemp();
		float getHeaterHighTemp();
	}
}
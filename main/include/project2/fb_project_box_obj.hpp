#pragma once



#include "fb_box.hpp"
#include "fb_sensor.hpp"
#include "fb_sensor_mp3.hpp"



namespace fb
{
	namespace project
	{
		void initBoxObjs();


		box::Box& getBox();
		box::Sensor& getBoxMp3Sensor();
		// box::Sensor& getBoxInsideSensor();
		// box::Sensor& getBoxOutsideSensor();
	}
}
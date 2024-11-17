#pragma once



#include "fb_box.hpp"
#include "fb_sensor.hpp"



namespace fb
{
	namespace project
	{
		void initBoxObjs();


		box::Box& getBox();
		box::Sensor& getBoxInsideSensor();
		box::Sensor& getBoxOutsideSensor();
	}
}
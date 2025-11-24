#pragma once



#include "fb_core.hpp"
#include "fb_sensor.hpp"



namespace fb
{
	namespace project
	{
		box::Sensor& getBoxInsideTempSensor();
		box::Sensor& getBoxInsideHumiditySensor();
		box::Sensor& getBoxOutsideTempSensor();
	}
}
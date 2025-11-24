#pragma once



#include "fb_project_base_box_obj.hpp"
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
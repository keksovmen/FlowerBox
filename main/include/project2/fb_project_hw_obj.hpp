#pragma once



#include "fb_sensor_service.hpp"
#include "fb_sensor_storage.hpp"
#include "fb_switch_service.hpp"
#include "fb_rgb_switch.hpp"
#include "fb_sensor_mp3.hpp"



namespace fb
{
	namespace project
	{
		void initHwObjs();

		sensor::Mp3Sensor& getHwMp3Sensor();

		switches::RgbSwitch& getHwRgbSwitch();

		sensor::SensorService& getHwSensorService();
		switches::SwitchService& getHwSwitchService();
		sensor::SensorStorage& getHwSensorStorage();
	}
}
#pragma once



#include "fb_rgb_switch.hpp"
#include "fb_sensor_keyboard.hpp"
#include "fb_sensor_mp3.hpp"
#include "fb_sensor_service.hpp"
#include "fb_sensor_storage.hpp"
#include "fb_switch_service.hpp"


namespace fb
{
	namespace project
	{
		void initHwObjs();

		sensor::Mp3Sensor& getHwMp3Sensor();
		sensor::KeyboardSensor<1>& getHwKeyboardSensor();

		switches::RgbSwitchDmx& getHwRgbSwitch();

		sensor::SensorService& getHwSensorService();
		switches::SwitchService& getHwSwitchService();
		sensor::SensorStorage& getHwSensorStorage();
	}
}
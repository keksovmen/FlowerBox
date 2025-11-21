#pragma once



#include "fb_http_puller.hpp"
#include "fb_sensor_keyboard.hpp"
#include "fb_sensor_service.hpp"
#include "fb_sensor_storage.hpp"
#include "fb_switch_service.hpp"



namespace fb
{
	namespace project
	{
		void initHwObjs();

		sensor::KeyboardSensor<1>& getHwKeyboardSensor();

		sensor::SensorService& getHwSensorService();
		switches::SwitchService& getHwSwitchService();
		sensor::SensorStorage& getHwSensorStorage();

		AbstractHttpPuller& getHwHttpPuller();
	}
}
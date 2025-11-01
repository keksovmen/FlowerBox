#pragma once



#include "fb_sensor_keyboard.hpp"
#include "fb_sensor_service.hpp"
#include "fb_sensor_storage.hpp"
#include "fb_switch_service.hpp"
#include "fb_gpio_switch.hpp"



namespace fb
{
	namespace project
	{
		void initHwObjs();

		sensor::KeyboardSensor<1>& getHwKeyboardSensor();

		sensor::SensorService& getHwSensorService();
		switches::SwitchService& getHwSwitchService();
		sensor::SensorStorage& getHwSensorStorage();

		switches::ArrayGpioSwitch<6>& getHwGpioSwitch();
	}
}
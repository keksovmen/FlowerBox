#pragma once



#include "fb_gpio_switch.hpp"
#include "fb_http_puller.hpp"
#include "fb_sensor_keyboard.hpp"
#include "fb_sensor_service.hpp"
#include "fb_sensor_storage.hpp"
#include "fb_switch_service.hpp"



namespace fb
{
	namespace project
	{
		sensor::KeyboardSensor<1>& getHwKeyboardSensor();
		switches::ArrayGpioSwitch<6>& getHwGpioSwitch();
		
		AbstractHttpPuller& getHwHttpPuller();
	}
}
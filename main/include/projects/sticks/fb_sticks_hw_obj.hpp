#pragma once



#include "fb_core_hw_obj.hpp"
#include "fb_http_puller.hpp"
#include "fb_sensor_keyboard.hpp"



namespace fb
{
	namespace project
	{
		sensor::KeyboardSensor<1>& getHwKeyboardSensor();

		AbstractHttpPuller& getHwHttpPuller();
	}
}
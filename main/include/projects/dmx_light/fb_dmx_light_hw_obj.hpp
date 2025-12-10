#pragma once



#include "fb_core.hpp"
#include "fb_rgb_switch.hpp"
#include "fb_sensor_keyboard.hpp"
#include "fb_sensor_mp3.hpp"
#include "fb_mqtt_client.hpp"



namespace fb
{
	namespace project
	{
		sensor::Mp3Sensor& getHwMp3Sensor();
		sensor::KeyboardSensor<1>& getHwKeyboardSensor();

		switches::RgbSwitchDmx& getHwRgbSwitch();

		periph::MqttClient& getHwMqttClient();
	}
}
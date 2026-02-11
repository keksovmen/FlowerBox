#pragma once



#include "fb_core.hpp"
#include "fb_dmx_hal.hpp"
#include "fb_mqtt_client.hpp"
#include "fb_sensor_keyboard.hpp"
#include "fb_sensor_mp3.hpp"



namespace fb
{
	namespace project
	{
		sensor::Mp3Sensor& getHwMp3Sensor();
		sensor::KeyboardSensor<1>& getHwKeyboardSensor();

		periph::DmxHal& getHwDmxHal();


		periph::MqttClient& getHwMqttClient();
	}
}
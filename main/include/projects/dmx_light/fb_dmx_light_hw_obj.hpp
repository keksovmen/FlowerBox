#pragma once



#include "fb_core.hpp"
#include "fb_dmx_hal.hpp"
#include "fb_mqtt_client.hpp"
#include "fb_sensor_keyboard.hpp"
#if _HW_VERSION == 2
	#include "fb_wrappers.hpp"
#else
	#include "fb_sensor_mp3.hpp"
#endif


namespace fb
{
	namespace project
	{
		#if _HW_VERSION == 2
			wrappers::WrapperGpio& getHwRelay();
		#else
			sensor::Mp3Sensor& getHwMp3Sensor();
		#endif
		
		sensor::KeyboardSensor<1>& getHwKeyboardSensor();

		periph::DmxHal& getHwDmxHal();


		periph::MqttClient& getHwMqttClient();
	}
}
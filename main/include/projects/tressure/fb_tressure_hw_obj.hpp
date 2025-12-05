#pragma once



#include "fb_core.hpp"
#include "fb_wrappers.hpp"
#include "fb_battery.hpp"
#include "fb_mqtt_client.hpp"



namespace fb
{
	namespace project
	{
		adc::AdcPin& getAdcPinHw();
		adc::Battery& getBatteryHw();
		wrappers::WrapperGpio& getPulseGpio();
		periph::MqttClient& getMqttClient();

		void doorIsOpened();
		void doorIsClosed();
	}
}
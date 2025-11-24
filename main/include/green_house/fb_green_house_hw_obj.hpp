#pragma once



#include "fb_core.hpp"
#include "fb_heat_switch.hpp"
#include "fb_sensor_aht20.hpp"
#include "fb_sensor_temperature.hpp"
#include "fb_time_switch.hpp"



namespace fb
{
	namespace project
	{
		sensor::TemperatureSensorArrayI& getHwTempSensors();
		sensor::TemperatureSensor& getHwDS18Sensor();
		sensor::SensorAht20& getHwAhtSensor();

		switches::TimeSwitch& getHwLightSwitch();
		switches::SensorSwitch& getHwHeatSwitch();
		switches::FanSwitch& getHwFanSwitch();
	}
}
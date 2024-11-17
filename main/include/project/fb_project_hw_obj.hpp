#pragma once



#include "fb_heat_switch.hpp"
#include "fb_sensor_service.hpp"
#include "fb_sensor_storage.hpp"
#include "fb_sensor_temperature.hpp"
#include "fb_switch_service.hpp"
#include "fb_time_switch.hpp"



namespace fb
{
	namespace project
	{
		void initHwObjs();

		sensor::TemperatureSensorArrayI& getHwTempSensors();
		sensor::TemperatureSensor& getHwInsideTempSensor();
		sensor::TemperatureSensor& getHwOutsideTempSensor();

		switches::TimeSwitch& getHwLightSwitch();
		switches::HeatSwitch& getHwHeatSwitch();
		switches::FanSwitch& getHwFanSwitch();

		sensor::SensorService& getHwSensorService();
		switches::SwitchService& getHwSwitchService();
		sensor::SensorStorage& getHwSensorStorage();
	}
}
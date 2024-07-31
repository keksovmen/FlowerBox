#pragma once



// #include "fb_sensor.hpp"



namespace fb
{
	namespace sensor
	{
		// class TemperatureSensor : public box::Sensor
		struct TemperatureSensor
		{
			using Id = unsigned long long;


			
			bool alive;
			float value;
			Id id;
		};

	} // namespace sensor
} // namespace fb

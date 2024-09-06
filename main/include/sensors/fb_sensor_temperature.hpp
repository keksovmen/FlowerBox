#pragma once



#include <vector>

#include "fb_sensor_iface.hpp"

#include "onewire.h"



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



		class TempreatureSensorTest : public SensorIface
		{
			public:
				TempreatureSensorTest(int gpio);
				~TempreatureSensorTest();

				virtual const char* getName() override;

			private:
				OW_t _interface;
				std::vector<TemperatureSensor> _sensors;



				virtual bool _doInit() override;
				virtual bool _doUpdate() override;

				void _temperatureMesureRequest();
				//считывает ответ
				float _tempreatureValueRequest(TemperatureSensor::Id id);
		};

	} // namespace sensor
} // namespace fb

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
				constexpr static float InvalidValue = -255.0f;



				TempreatureSensorTest(int gpio);
				~TempreatureSensorTest();

				virtual const char* getName() override;
				
				float getValue() const;

			private:
				OW_t _interface;
				std::vector<TemperatureSensor> _sensors;

				float _value = InvalidValue;



				virtual bool _doInit() override;
				virtual bool _doUpdate() override;

				void _temperatureMesureRequest();
				//считывает ответ
				float _tempreatureValueRequest(TemperatureSensor::Id id);
		};

	} // namespace sensor
} // namespace fb

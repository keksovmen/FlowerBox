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



				TempreatureSensorTest(int gpio, int expectedDevices);
				~TempreatureSensorTest();

				virtual const char* getName() override;
				
				float getValue(int index) const;
				int getDeviceCount() const;

			private:
				OW_t _interface;
				int _expectedDevices;
				std::vector<TemperatureSensor> _sensors;


				virtual bool _doInit() override;
				virtual bool _doUpdate() override;

				void _temperatureMesureRequest();
				//считывает ответ
				float _tempreatureValueRequest(TemperatureSensor::Id id);
		};

	} // namespace sensor
} // namespace fb

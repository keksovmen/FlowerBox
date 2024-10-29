#pragma once



#include <vector>

#include "fb_sensor_iface.hpp"

#include "onewire.h"



namespace fb
{
	namespace sensor
	{
		class TemperatureSensorArray;



		class TemperatureSensor : public SensorIface
		// struct TemperatureSensor
		{
			public:
				friend class TemperatureSensorArray;



				using Id = unsigned long long;



				constexpr static Id InvalidId = -1;
				constexpr static float InvalidValue = -255.0f;



				TemperatureSensor(OW_t& interface);

				virtual const char* getName() override;

				float getValue() const;
				Id getId() const;

				void init(Id id);

				bool alive = false;
				float value = InvalidValue;
				Id id = InvalidId;

			private:
				OW_t& _interface;
				Id _id = InvalidId;
				float _value = InvalidValue;
				bool _alive = false;


				//check _id field
				virtual bool _doInit() override;
				//make value request
				virtual bool _doUpdate() override;

				void _temperatureMeasureRequest();
				float _temperatureValueRequest();

				void _resetState();
				void _alive();
		};



		class TemperatureSensorArray : public SensorIface
		{
			public:
				TemperatureSensorArray(int gpio, int expectedDevices);
				~TemperatureSensorArray();

				virtual const char* getName() override;

				int getDeviceCount() const;
				const TemperatureSensor& getSensor(int index) const;

			private:
				OW_t _interface;
				std::vector<TemperatureSensor> _sensors;



				virtual bool _doInit() override;
				virtual bool _doUpdate() override;
		};



		//TODO: made it accept OW_t interface as bus, and index of sensor on that bus, to have
		//2 instances for ease of BOX api providing
		class TempreatureSensorTest : public SensorIface
		{
			public:
				constexpr static float InvalidValue = -255.0f;



				TempreatureSensorTest(int gpio, int expectedDevices);
				~TempreatureSensorTest();

				virtual const char* getName() override;
				
				float getValue(int index) const;
				int getDeviceCount() const;
				const TemperatureSensor& getSensor(int index) const;

			private:
				OW_t _interface;
				int _expectedDevices;
				std::vector<TemperatureSensor> _sensors;


				virtual bool _doInit() override;
				virtual bool _doUpdate() override;

				void _temperatureMeasureRequest();
				//считывает ответ
				float _temperatureValueRequest(TemperatureSensor::Id id);
		};

	} // namespace sensor
} // namespace fb

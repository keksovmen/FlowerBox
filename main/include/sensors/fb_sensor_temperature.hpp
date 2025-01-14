#pragma once


#include <array>

#include "fb_sensor_iface.hpp"

#include "onewire.h"



namespace fb
{
	namespace sensor
	{
		template<int N>
		class TemperatureSensorArray;



		class TemperatureSensor : public SensorIface
		{
			public:
				using Id = unsigned long long;



				constexpr static Id InvalidId = -1;
				constexpr static float InvalidValue = 85.0f;



				virtual const char* getName() const override;

				void lateInit(Id id, OW_t* interface);

				float getValue() const;
				Id getId() const;

			private:
				OW_t* _interface;
				Id _id = InvalidId;
				float _value = InvalidValue;


				virtual bool _doInit() override;
				virtual SensorIface::UpdateResult _doUpdate() override;

				void _temperatureMeasureRequest();
				float _temperatureValueRequest();
		};



		class TemperatureSensorArrayI : public SensorIface
		{
			public:
				virtual int getDeviceCount() const = 0;
				virtual TemperatureSensor* getSensor(int index) = 0;
		};



		template<int N>
		class TemperatureSensorArray : public TemperatureSensorArrayI
		{
			public:
				TemperatureSensorArray(int gpio);
				~TemperatureSensorArray();

				virtual const char* getName() const override;

				virtual int getDeviceCount() const override;
				virtual TemperatureSensor* getSensor(int index) override;

				const TemperatureSensor& getSensor(int index) const;

			private:
				OW_t _interface;
				std::array<TemperatureSensor, N> _sensors;



				virtual bool _doInit() override;
				virtual SensorIface::UpdateResult _doUpdate() override;
		};



		template class TemperatureSensorArray<1>;
		// template class TemperatureSensorArray<2>;
	} // namespace sensor
} // namespace fb

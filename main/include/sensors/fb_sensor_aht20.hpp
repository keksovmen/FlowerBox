#pragma once



#include "fb_sensor_iface.hpp"

#include "aht20.h"



namespace fb
{
	namespace sensor
	{
		class SensorAht20 : public SensorIface
		{
			public:
				constexpr static float InvalidValue = -255.0f;



				SensorAht20(i2c_port_t port, int gpioSda, int gpioScl);
				~SensorAht20();

				virtual const char* getName() const override;

				float getTemperature() const;
				float getHumidity() const;
			
			private:
				i2c_master_bus_handle_t _bus = nullptr;
				aht20_dev_handle_t _hndl = nullptr;

				float _temperature = InvalidValue;
				float _humidity = InvalidValue;



				virtual bool _doInit() override;
				virtual SensorIface::UpdateResult _doUpdate() override;

		};
	}
}
#pragma once



#include <vector>

#include "fb_service_iface.hpp"
#include "fb_sensor_event.hpp"
#include "fb_sensor_temperature.hpp"
#include "fb_sensor_iface.hpp"




namespace fb
{
	namespace sensor
	{
		class SensorService : public util::ServiceIface
		{
			public:
				virtual const char* getName() override;

				void addSensor(SensorIface* entry);
			
				std::vector<SensorIface*> getSensors() const;
			
			private:
				std::vector<SensorIface*> _sensors;
				SemaphoreHandle_t _mutex;



				virtual void _onPull() override;

				void _dropEvent(SensorEvent e, SensorIface* data);
		};
	}
}
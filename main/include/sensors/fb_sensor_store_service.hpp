#pragma once



#include "fb_debug.hpp"
#include "fb_event_manager.hpp"
#include "fb_sensor_storage.hpp"



namespace fb
{
	namespace sensor
	{
		class SensorStoreService : public event::EventListener, public debug::Named
		{
			public:
				SensorStoreService(sensor::SensorStorage& storage);

				virtual void handleEvent(const event::Event& event) override;
				virtual const char* getName() const override;
			
			private:
				sensor::SensorStorage& _storage;



				void _handleChangeEvent(sensor::SensorIface* sensor);

				bool _isSupportedSensor(const sensor::SensorIface* sensor) const;
				float _mapSensorToPrecision(const sensor::SensorIface* sensor, int valueIndex) const;
		};
	} // namespace box
	
}
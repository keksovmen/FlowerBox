#pragma once



#include <functional>

#include "fb_debug.hpp"
#include "fb_event_manager.hpp"
#include "fb_sensor_iface.hpp"
#include "fb_sensor_storage.hpp"



namespace fb
{
	namespace sensor
	{
		struct SensorStoreCfg
		{
			sensor::SensorIface* sens;
			std::function<float(int)> mapIndexToPrecision;
		};



		class SensorStoreService : public event::EventListener, public debug::Named
		{
			public:
				SensorStoreService(sensor::SensorStorage& storage);

				virtual void handleEvent(const event::Event& event) override;
				virtual const char* getName() const override;

				void registerSensor(const SensorStoreCfg& sens);
			
			private:
				sensor::SensorStorage& _storage;

				std::vector<SensorStoreCfg> _regedSensors;



				void _handleChangeEvent(sensor::SensorIface* sensor);

				bool _isSupportedSensor(const sensor::SensorIface* sensor) const;
				float _mapSensorToPrecision(const sensor::SensorIface* sensor, int valueIndex) const;
		};
	} // namespace box
	
}
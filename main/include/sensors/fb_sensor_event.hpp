#pragma once



namespace fb
{
	namespace sensor
	{
		enum class SensorEvent : int
		{
			TEMPERATURE_SENSOR_SCANNED,			//когда закончили сканировать
			TEMPERATURE_SENSOR_DETECTED,		//когда нашли устройство во время скана
			TEMPERATURE_SENSOR_VALUE_CHANGED,	//у устройства изменилось значение
			TEMPERATURE_SENSOR_LOST,			//устройство потеряно
		};

		constexpr bool operator==(int id, SensorEvent e){
			return id == static_cast<int>(e);
		}

	} // namespace sensor
	
} // namespace fb

#pragma once



namespace fb
{
	namespace sensor
	{
		enum class SensorEvent : int
		{
			SENSOR_INITIALIZED,					//сенсор инициализирован, аргумент есть сенсор
			SENSOR_LOST,						//сенсор потерян, по какой либо причине
			SENSOR_VALUE_CHANGED,				//сенсор изменил свое значение
		};

		constexpr bool operator==(int id, SensorEvent e){
			return id == static_cast<int>(e);
		}

	} // namespace sensor
	
} // namespace fb

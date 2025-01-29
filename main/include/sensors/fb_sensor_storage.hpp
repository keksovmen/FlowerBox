#pragma once



#include <cstddef>
#include <iterator>
#include <unordered_map>
#include <optional>

#include "fb_sensor_temperature.hpp"
#include "fb_clock.hpp"
#include "fb_cycle_buffer.hpp"



namespace fb
{
	namespace sensor
	{
		struct SensorStorageEntry
		{
			float value;
			clock::Timestamp timestamp;
		};



		class SensorStorage
		{
			public:
				static constexpr int BufferSize = 50;



				/**
				 * @brief то чем мы будем проходится по списку сохраненных данныъ
				 * 
				 */

				using Iterator = util::CycleBuffer<SensorStorageEntry, BufferSize>::Iterator;



				/**
				 * @brief добавит значение с текущем временем
				 * 
				 * @param address датчика
				 * @param value значение
				 */

				//TODO: change address to arbitrary index
				
				void addSensorValue(int address, float value);
				// void addSensorState(int address, bool state);

				/**
				 * @brief возвращаем итератор на желаеммые данные
				 * 
				 * @param address датчика
				 * @param from после какого промежутка времени возвращать
				 * @return Iterator
				 */

				Iterator getSensorValues(int address, clock::Timestamp from) const;

				/**
				 * @brief возвращает конечный итератор end()
				 * 
				 * @return Iterator 
				 */

				Iterator getSensorValuesEnd(int address) const;

				/**
				 * @brief достает, если есть, последнее сохраненное значение
				 * 
				 * @param address датчика
				 * @return std::optional<float> 
				 */

				std::optional<float> getSensorLastValue(int address) const;
			
			private:
				static constexpr int _ILLEGAL_INDEX = -1;
				static constexpr int _MISSING_BUFFER_INDEX = 0;



				using Buffer = util::CycleBuffer<SensorStorageEntry, BufferSize>;


				//TODO: make universal
				std::array<Buffer, 3> _sensorData;
				std::unordered_map<int, int> _addressMap;



				Buffer* _getSensorValueBuffer(int address);
				const Buffer* _getSensorValueBuffer(int address) const;
				int _mapAddresToIndex(int address) const;
				int _addAddres(int address);
		};
	}
}
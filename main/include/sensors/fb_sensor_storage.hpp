#pragma once



#include <cstddef>
#include <iterator>

#include "fb_sensor_temperature.hpp"
#include "fb_time.hpp"
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
				static constexpr int BufferSize = 10;
				// class SensorStorageIterator
				// {
				// 	public:

				// 	using iterator_category = std::forward_iterator_tag;
				// 	using value_type = SensorStorageEntry;
				// 	using element_type = SensorStorageEntry;
				// 	using pointer = SensorStorageEntry*;
				// 	using reference = SensorStorageEntry&;
				// 	using difference_type = std::ptrdiff_t;

				// 	SensorStorageIterator()
				// 			: _data(data), _index(index)
				// 	{

				// 	}

				// 	const reference operator*()
				// 	{
				// 		return _data[_index];
				// 	}

				// 	bool operator==(const SensorStorageIterator& r)
				// 	{
				// 		return _index == r._index;
				// 	}

				// 	SensorStorageIterator& operator++()
				// 	{
				// 		return *this;
				// 	}

				// };



				/**
				 * @brief то чем мы будем проходится по списку сохраненных данныъ
				 * 
				 */
				//TODO: change to iterator in cycle buffer
				using Iterator = util::CycleBuffer<SensorStorageEntry, BufferSize>::Iterator;



				/**
				 * @brief добавит значение с текущем временем
				 * 
				 * @param id датчика
				 * @param value значение
				 */
				
				void addSensorValue(TemperatureSensor::Id id, float value);
				void addSensorState(TemperatureSensor::Id id, bool state);

				/**
				 * @brief возвращаем итератор на желаеммые данные
				 * 
				 * @param id датчика
				 * @param from после какого промежутка времени возвращать
				 * @return Iterator
				 */

				Iterator getSensorValues(TemperatureSensor::Id id, clock::Timestamp from) const;

				/**
				 * @brief возвращает конечный итератор end()
				 * 
				 * @return Iterator 
				 */

				Iterator getSensorValuesEnd() const;
			
			private:
				using Buffer = util::CycleBuffer<SensorStorageEntry, BufferSize>;

				Buffer _sensorData;



				Buffer& _getSensorValueBuffer(TemperatureSensor::Id id);


		};
	}
}
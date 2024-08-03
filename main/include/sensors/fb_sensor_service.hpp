#pragma once



#include <unordered_map>
#include <vector>
#include <functional>

#include "fb_debug.hpp"
#include "fb_sensor_event.hpp"
#include "fb_sensor_temperature.hpp"
// #include "fb_subject.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"

#include "onewire.h"



namespace fb
{
	namespace sensor
	{
		class SensorService : public debug::Named
		{
			public:
				virtual const char* getName() override;

				void start(int gpio);
				void stop();

				void forseScan();
				void forseRead();
			
				//вернет копию, дабы синхронизацию не ломать
				std::vector<TemperatureSensor> getSensors() const;
			
			private:
				using Action = std::function<void(SensorService*)>;



				std::unordered_map<TemperatureSensor::Id, TemperatureSensor> _tempSensorList;
				SemaphoreHandle_t _mutex;
				QueueHandle_t _actionQueue;
				TaskHandle_t _taskHndl;
				TimerHandle_t _timerHndl;
				OW_t _wireIface;



				//задача
				static void _task(void* arg);
				static void _scanAction(SensorService* me);
				static void _temperatureAction(SensorService* me);
				static void _timer(TimerHandle_t timer);

				std::vector<TemperatureSensor::Id> _scanRequest();
				//занимает время для сенсоров для формирования ответа
				void _temperatureMesureRequest();
				//считывает ответ
				float _tempreatureValueRequest(TemperatureSensor::Id id);

				//меняет состояние сенсоров, если таких нет то добавляет, если есть, то ставит живым, а если нет то мертвым
				void _updateSensorStates(const std::vector<TemperatureSensor::Id>& alive);
				//меняет значение температуры сенсора
				void _updateTemperatureValue(TemperatureSensor::Id id, float value);

				void _dropEvent(SensorEvent e, TemperatureSensor* data);
		};
	}
}
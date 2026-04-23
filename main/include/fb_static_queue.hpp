#pragma once



#include <vector>

#include "fb_lock_wrapper.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"



namespace fb
{
	namespace util
	{
		template<int S, typename T>
		class StaticQueue 
		{
			static_assert(S > 0, "Queue size must be positive");

		public:
			StaticQueue() {
				_mutex = xSemaphoreCreateMutex();
				_count = 0;
			}

			~StaticQueue() {
				if (_mutex != nullptr) {
					vSemaphoreDelete(_mutex);
				}
			}

			// Disable copy and assignment (_mutex is not copyable)
			StaticQueue(const StaticQueue&) = delete;
			StaticQueue& operator=(const StaticQueue&) = delete;

			// Add an entry to the queue. Ignore if full.
			void addEntry(const T& entry) {
				util::LockWrapper lock(_mutex, portMAX_DELAY);
				if (lock) {
					if (_count < S) {
						_buffer[_count++] = entry;
					}
					// else ignore: queue is full
				}
			}

			// Read all entries and reset the queue.
			std::vector<T> readQueue() {
				std::vector<T> result;
				util::LockWrapper lock(_mutex, portMAX_DELAY);
				if(lock){
					result.reserve(_count);
					for (int i = 0; i < _count; ++i) {
						result.push_back(_buffer[i]);
					}
					_count = 0;  // reset state
				}
				return result;
			}

		private:
			T _buffer[S];
			int _count;
			SemaphoreHandle_t _mutex;
		};
	}
}
#pragma once



#include <array>
#include <algorithm>
#include <functional>
#include <memory>



namespace fb
{
	namespace subject
	{
		template<class T>
		class Subject
		{
			public:
				using EventCb = std::function<void(T* listener)>;
				// using Value = std::shared_ptr<T>;
				using Value = T*;



				virtual void attachListener(Value listener) = 0;
				virtual void detachListener(const T* listener) = 0;
				virtual void fireEvent(const EventCb& cb) = 0;
		};



		template<class T, size_t N>
		class BaseSubject : public Subject<T>
		{
			public:
				virtual void attachListener(Subject<T>::Value listener) override
				{
					assert(_listenersCount < _listeners.size());
					_listeners[_listenersCount] = listener;
					_listenersCount++;
				}

				virtual void detachListener(const T* listener) override
				{
					auto iter = std::remove_if(_listeners.begin(), _listeners.begin() + _listenersCount,
						[listener](const Subject<T>::Value& val){
							return val == listener;
					});

					if(iter != (_listeners.begin() + _listenersCount)){
						_listenersCount--;
					}
				}

				virtual void fireEvent(const Subject<T>::EventCb& cb) override
				{
					std::for_each(_listeners.begin(), _listeners.begin() + _listenersCount,
						[cb](Subject<T>::Value& v){cb(v);});
				}

			private:
				std::array<typename Subject<T>::Value, N> _listeners;
				int _listenersCount = 0;
		};
	}
}
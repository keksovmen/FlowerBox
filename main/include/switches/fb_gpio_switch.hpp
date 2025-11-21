#pragma once



#include <array>
#include "fb_switch_iface.hpp"



namespace fb
{
	namespace switches
	{
		class GpioSwitch : public SwitchIface
		{
			public:
				GpioSwitch(int gpio) : 
					SwitchIface(SwitchIface::MockCondition, &_action), 
					_gpio(gpio)
				{

				}

				virtual const char* getName() const override
				{
					return "GpioSwitch";
				}

				void turnOn();
				void turnOff();

				bool isOn() const
				{
					return _state;
				}

				int getGpio() const
				{
					return _gpio;
				}

			private:
				const int _gpio;
				bool _state = false;
				bool _initFlag = false;



				static void _action(SwitchIface* me, bool state);



				void _init();
		};



		template<int N>
		class ArrayGpioSwitch : public SwitchIface
		{
			public:
				ArrayGpioSwitch(std::array<GpioSwitch, N> gpios) :
					SwitchIface(SwitchIface::MockCondition, &_action),
					_gpios(std::move(gpios))
				{
				}

				virtual const char* getName() const override
				{
					return "ArrayGpioSwitch";
				}

				void turnOn(int index)
				{
					if(!_hasIndex(index)) return;

					_gpios[index].turnOn();
				}

				void turnOnAll()
				{
					for(auto& g : _gpios){
						g.turnOn();
					}
				}

				void turnOff(int index)
				{
					if(!_hasIndex(index)) return;

					_gpios[index].turnOff();
				}

				void turnOffAll()
				{
					for(auto& g : _gpios){
						g.turnOff();
					}
				}

				bool isOn(int index) const
				{
					if(!_hasIndex(index)) return false;

					return _gpios[index].isOn();
				}

				int getGpio(int index) const
				{
					if(!_hasIndex(index)) return -1;

					return _gpios[index].getGpio();
				}

			private:
				std::array<GpioSwitch, N> _gpios;



				bool _hasIndex(int index) const
				{
					return index >= 0 && index < N;
				}


				static void _action(SwitchIface* me, bool state)
				{
					ArrayGpioSwitch* tmp = static_cast<ArrayGpioSwitch*>(me);
					if(state){
						tmp->turnOnAll();
					}else{
						tmp->turnOffAll();
					}
				}
		};
	}
}
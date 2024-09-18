#pragma once



#include "fb_clock.hpp"
#include "fb_switch_iface.hpp"



namespace fb
{
	namespace switches
	{
		class TimeSwitch : public SwitchIface
		{
			public:
				TimeSwitch(
					const clock::Time& startTime,
					const clock::Time& duration,
					int gpio);

				virtual const char* getName() override;

			
			private:
				clock::Time _startTime;
				clock::Time _duration;
				int _gpio;



				static bool _condition(SwitchIface* me);
				static void _action(SwitchIface* me, bool value);
		};
	}
}
#pragma once



#include "driver/ledc.h"

#include "fb_switch_iface.hpp"



namespace fb
{
	namespace switches
	{
		class RgbSwitch : public SwitchIface
		{
			public:
				RgbSwitch(ledc_timer_t timer, ledc_channel_t startChannel, int gpioR, int gpioG, int gpioB);

				virtual const char* getName() const override;

				void setColor(int color);
				int getColor() const;

			private:
				int _color{0};
				ledc_timer_t _timer{};
				ledc_channel_t _startChannel{};


				void _applyColor(int color);

				static bool _condition(SwitchIface* me);
				static void _action(SwitchIface* me, bool value);

		};
	}
}
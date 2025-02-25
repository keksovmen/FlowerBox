#pragma once



#include "fb_switch_iface.hpp"

#include "driver/ledc.h"

#include "esp_dmx.h"



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



		class RgbSwitchDmx : public SwitchIface
		{
			public:
				RgbSwitchDmx(dmx_port_t port, int gpioRX, int gpioTX, int gpioRTS);
				~RgbSwitchDmx();

				virtual const char* getName() const override;

				void init();
				void setColor(int color);
				int getColor() const;

			private:
				const dmx_port_t _dmx;
				const int _gpioRX, _gpioTX, _gpioRTS;

				int _color{0};
				int _address{1};



				void _applyColor(int color);

				static bool _condition(SwitchIface* me);
				static void _action(SwitchIface* me, bool value);
		};
	}
}
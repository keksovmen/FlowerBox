#pragma once



#include "fb_switch_iface.hpp"



namespace fb
{
	namespace switches
	{
		class RgbSwitch : public SwitchIface
		{
			public:
				RgbSwitch(int gpioR, int gpioG, int gpioB);

				virtual const char* getName() const override;

				void setColor(int color);
				int getColor() const;

			private:
				static bool _condition(SwitchIface* me);
				static void _action(SwitchIface* me, bool value);

		};
	}
}
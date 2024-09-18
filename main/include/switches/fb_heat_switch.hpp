#pragma once



#include "fb_switch_iface.hpp"
#include "fb_sensor_temperature.hpp"



namespace fb
{
	namespace switches
	{
		//TODO: made heat switch
		class HeatSwitch : public SwitchIface
		{
			public:
				HeatSwitch(sensor::TempreatureSensorTest* sensor,
					float lowTemp, float highTemp, int gpio);
				~HeatSwitch();

				virtual const char* getName() override;

			private:
				const sensor::TempreatureSensorTest* _sensor;

				float _lowTemp;
				float _highTemp;
				int _gpio;
				bool _colling = false;



				static bool _condition(SwitchIface* me);
				static void _action(SwitchIface* me, bool value);
		};

	} // namespace switches
} // namespace fb

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
				HeatSwitch(sensor::TemperatureSensor* sensor,
					float lowTemp, float highTemp, int gpio);
				virtual ~HeatSwitch();

				virtual const char* getName() override;

				float getLowTemp() const;
				float getHighTemp() const;
				int getGpio() const;
				const sensor::TemperatureSensor* getSensor() const;


			protected:
				virtual bool _checkTemperature();

				void _setColling(bool state);
				bool _isColling() const;
				float _getSensorValue() const;

			private:
				const sensor::TemperatureSensor* _sensor;

				float _lowTemp;
				float _highTemp;

				int _gpio;
				bool _colling = false;



				static bool _condition(SwitchIface* me);
				static void _action(SwitchIface* me, bool value);
		};



		class FanSwitch : public HeatSwitch
		{
			public:
				FanSwitch(sensor::TemperatureSensor* sensor,
						float lowTemp, float highTemp, int gpio);

				virtual const char* getName() override;
			
			protected:
				virtual bool _checkTemperature();

		};
	} // namespace switches
} // namespace fb

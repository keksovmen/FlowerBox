#pragma once



#include "fb_event_manager.hpp"
#include "fb_debug.hpp"



namespace fb
{
	namespace pins
	{
		//data pin
		static constexpr int PIN_SENSOR_DS18 = 10;

		//red LED, controls  heating
		static constexpr int PIN_HEATER = 3;
		static constexpr int PIN_RED_LED = 3;

		//green LED, controls light
		static constexpr int PIN_LIGHT = 4;
		static constexpr int PIN_GREEN_LED = 4;

		//fan pin, PWM, also blue LED
		static constexpr int PIN_FAN = 5;
		static constexpr int PIN_BLUE_LED = 5;

		static constexpr int PIN_COOL_LED = 19;

		//AHT20 IIC interface
		static constexpr int PIN_IIC_SDA = 7;
		static constexpr int PIN_IIC_SCL = 8;



		// class PinManager : public debug::Named, public event::EventListener
		// {
		// 	public:
		// 		virtual const char* getName() const override;
		// 		virtual void handleEvent(const event::Event& event) override;
			
		// 	private:
		// 		void _onBootEvent(const event::Event& event);
		// };

	}
}
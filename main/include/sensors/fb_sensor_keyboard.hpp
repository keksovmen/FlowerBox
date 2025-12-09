#pragma once



#include <array>
#include <utility>

#include "fb_sensor_iface.hpp"
#include "h_keyboard.hpp"



namespace fb
{
	namespace sensor
	{
		enum class KeyboardEventId : int
		{
			ACTION = 0,
		};



		template<int N>
		class KeyboardSensor : public SensorIface
		{
			public:
				using PinList = std::array<std::pair<int, h::ButtonVK>, N>;



				constexpr KeyboardSensor(PinList pins)
					: _pins(pins)
				{

				}

				virtual const char* getName() const override;

			private:
				PinList _pins;



				virtual bool _doInit() override;
				virtual SensorIface::UpdateResult _doUpdate() override;
		};



		template<int N>
		class KeyboardSensorIrq : public SensorIface
		{
			public:
				using PinList = std::array<std::pair<int, h::ButtonVK>, N>;



				constexpr KeyboardSensorIrq(PinList pins)
					: _pins(pins)
				{

				}

				virtual const char* getName() const override;

			private:
				PinList _pins;



				virtual bool _doInit() override;
				virtual SensorIface::UpdateResult _doUpdate() override;
		};



		template class KeyboardSensor<1>;
		template class KeyboardSensor<2>;
		template class KeyboardSensorIrq<1>;
		template class KeyboardSensorIrq<2>;
	}
}
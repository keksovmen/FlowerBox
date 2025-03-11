#pragma once



#include "fb_switch_iface.hpp"
#include "fb_sensor_temperature.hpp"
#include "fb_wrappers.hpp"



namespace fb
{
	namespace switches
	{
		class RangeSwitch : public SwitchIface
		{
			public:
				using ReadCb = std::function<float()>;
				using ActionCb = std::function<void(bool)>;



				static constexpr float INVALID_VALUE = -255.0f;



				RangeSwitch(float lowValue, float highValue, ReadCb read, ActionCb action);
				virtual ~RangeSwitch() = default;

				virtual const char* getName() const override;

				float getLowValue() const;
				float getHighValue() const;

			protected:
				virtual bool _checkValues();

				void _setColling(bool state);
				bool _isColling() const;
				float _getSensorValue() const;

			private:
				const ReadCb _readCb;
				const ActionCb _actionCb;

				float _lowValue;
				float _highValue;

				bool _colling = false;



				static bool _condition(SwitchIface* me);
				static void _action(SwitchIface* me, bool value);
		};



		class HeatSwitch : public RangeSwitch
		{
			public:
				HeatSwitch(sensor::SensorIface* sensor,
					float lowTemp, float highTemp, wrappers::WrapperIface* wrapper);

				virtual const char* getName() const override;

				/**
				 * @brief Set the Speed
				 * 
				 * @param speed from 0 to 100 inclusive
				 */

				void setSpeed(int speed);

				int getSpeed() const;

			private:
				const sensor::SensorIface* _sensor;
				wrappers::WrapperIface* _wrapper;
				int _speed = 1;
		};



		class FanSwitch : public HeatSwitch
		{
			public:
				FanSwitch(sensor::SensorIface* sensor,
					float lowTemp, float highTemp, wrappers::WrapperIface* wrapper);

				virtual const char* getName() const override;
			
			protected:
				virtual bool _checkValues() override;

		};
	} // namespace switches
} // namespace fb

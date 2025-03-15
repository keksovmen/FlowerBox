#pragma once



#include "fb_clock.hpp"
#include "fb_sensor_aht20.hpp"
#include "fb_switch_iface.hpp"
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



				RangeSwitch(float lowValue, float highValue, ReadCb read, ActionCb action, bool inverseFlag = false);
				virtual ~RangeSwitch() = default;

				virtual const char* getName() const override;

				void setLowValue(float value);
				void setHighValue(float value);

				float getLowValue() const;
				float getHighValue() const;

				bool checkValues();

			protected:
				void _setColling(bool state);
				bool _isColling() const;
				float _getSensorValue() const;

				virtual float _getTargetLowValue() const;
				virtual float _getTargetHighValue() const;

			private:
				const ReadCb _readCb;
				const ActionCb _actionCb;
				const bool _inverseFlag;

				float _lowValue;
				float _highValue;

				bool _colling = false;



				static bool _condition(SwitchIface* me);
				static void _action(SwitchIface* me, bool value);
		};



		class DayNightRangeSwitch : public RangeSwitch
		{
			public:
				DayNightRangeSwitch(float lowValue, float highValue,
							ReadCb read, ActionCb action,
							int delta, bool inverseFlag = false);

				virtual const char* getName() const override;

				void setDelta(float delta);
				float getDelta() const;

				void setDayStartTime(clock::Timestamp seconds);
				void setDayEndTime(clock::Timestamp seconds);

				const clock::Time& getDayStartTime() const;
				const clock::Time& getDayEndTime() const;
			
			protected:
				virtual float _getTargetLowValue() const override;
				virtual float _getTargetHighValue() const override;
			
			private:
				float _delta;
				clock::Time _startTime{0};
				clock::Time _endTime{0};



				bool _isDay() const;
		};



		class SensorSwitch : public DayNightRangeSwitch
		{
			public:
				SensorSwitch(sensor::SensorIface* sensor,
					float lowTemp, float highTemp, wrappers::WrapperIface* wrapper, bool inverseFlag = false);

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



		class FanSwitch : public SwitchIface
		{
			public:
				FanSwitch(sensor::SensorAht20* sensor,
					float lowTemp, float highTemp,
					float lowHumidity, float highHumidity,
					wrappers::WrapperIface* wrapper);
				
				virtual const char* getName() const override;

				void setSpeed(int speed);

				int getSpeed() const;

				void setTempLowValue(float value);
				void setTempHighValue(float value);
				void setHumLowValue(float value);
				void setHumHighValue(float value);

				float getTempLowValue() const;
				float getTempHighValue() const;
				float getHumLowValue() const;
				float getHumHighValue() const;

				void setDayStartTime(clock::Timestamp seconds);
				void setDayEndTime(clock::Timestamp seconds);

				const clock::Time& getDayStartTime() const;
				const clock::Time& getDayEndTime() const;

				void setDelta(float delta);
				float getDelta() const;
			
			private:
				const sensor::SensorAht20* _sensor;
				wrappers::WrapperIface* _wrapper;

				RangeSwitch _tempSwitch;
				DayNightRangeSwitch _humSwitch;

				int _speed = 100;



				static bool _condition(SwitchIface* me);
				static void _action(SwitchIface* me, bool value);
		};
	} // namespace switches
} // namespace fb

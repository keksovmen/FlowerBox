#pragma once



#include "fb_clock.hpp"
#include "fb_switch_iface.hpp"
#include "fb_wrappers.hpp"



namespace fb
{
	namespace switches
	{
		class TimeSwitch : public SwitchIface
		{
			public:
				TimeSwitch(
					const clock::Time& startTime,
					const clock::Time& endTime,
					wrappers::WrapperIface* wrapper);

				virtual const char* getName() const override;

				void setStartTime(clock::Timestamp seconds);
				void setEndTime(clock::Timestamp seconds);

				const clock::Time& getStartTime() const;
				const clock::Time& getEndTime() const;

			private:
				clock::Time _startTime;
				clock::Time _endTime;
				wrappers::WrapperIface* _wrapper;


				bool _isOverflow() const;

				static bool _condition(SwitchIface* me);
				static void _action(SwitchIface* me, bool value);
		};
	}
}
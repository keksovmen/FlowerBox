#pragma once



#include <functional>

#include "fb_debug.hpp"



namespace fb
{
	namespace util
	{
		class Counter : public debug::Named
		{
			public:
				using OnAchieveTarget = std::function<void(int target)>;



				Counter(int target, int value, OnAchieveTarget targetCb);

				virtual const char* getName() const override;

				void changeMultiplier();
				void increment();
				void setTarget(int target);
				void setValue(int value);

				int getValue() const;
			
			private:
				const OnAchieveTarget _resultCb;

				int _target;
				int _value;
				int _multiplier = 1;



				void _checkCondition();
		};
	}
}
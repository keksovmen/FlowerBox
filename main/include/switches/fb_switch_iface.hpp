#pragma once



#include <concepts>
#include <functional>
#include "fb_debug.hpp"



namespace fb
{
	namespace switches
	{
		// template<class T>
		class SwitchIface : public debug::Named
		{
			public:
				/**
				 * @brief 
				 * @param this
				 * @return true -> action will be performed
				 * 			false -> no action
				 */

				using ConditionCb = std::function<bool(SwitchIface*)>;
				using ActionCb = std::function<void(SwitchIface*, bool)>;



				SwitchIface(const ConditionCb& conditionCb, const ActionCb actionCb);
				virtual ~SwitchIface();

				void check();

				bool isOn() const;

			
			private:
				const ConditionCb _conditionCb;
				const ActionCb _actionCb;

				bool _state = false;
		};

	} // namespace switches
} // namespace fb

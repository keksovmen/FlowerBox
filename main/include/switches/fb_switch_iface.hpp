#pragma once



#include <functional>
#include "fb_debug.hpp"



namespace fb
{
	namespace switches
	{
		enum class SwitchForseState : int
		{
			NONE = 0,
			ON,
			OFF
		};



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



				static constexpr auto MockCondition = [](SwitchIface* me){return false;};
				static constexpr auto MockAction = [](SwitchIface* me, bool){};



				SwitchIface(const ConditionCb& conditionCb, const ActionCb actionCb);
				virtual ~SwitchIface() = default;

				void check();
				void setForseFlag(SwitchForseState state);

				bool isOn() const;

			
			private:
				const ConditionCb _conditionCb;
				const ActionCb _actionCb;

				bool _state = false;
				SwitchForseState _forseFlag = SwitchForseState::NONE;
		};

	} // namespace switches
} // namespace fb

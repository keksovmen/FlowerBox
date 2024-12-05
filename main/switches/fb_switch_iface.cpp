#include "fb_switch_iface.hpp"



using namespace fb;
using namespace switches;



SwitchIface::SwitchIface(const ConditionCb &conditionCb, const ActionCb actionCb)
	: _conditionCb(conditionCb), _actionCb(actionCb)
{
}

void SwitchIface::check()
{
	if(_forseFlag == SwitchForseState::ON){
		if(!_state){
			FB_DEBUG_LOG_I_OBJ("[%s] forse flag is ON -> action", getName());

			_actionCb(this, true);
			_state = true;		
		}

		return;
	}

	if(_forseFlag == SwitchForseState::OFF){
		if(_state){
			FB_DEBUG_LOG_I_OBJ("[%s] forse flag is OFF -> action", getName());

			_actionCb(this, false);
			_state = false;		
		}

		return;
	}

	if (_conditionCb(this))
	{
		if (!_state)
		{
			FB_DEBUG_LOG_I_OBJ("[%s] condition met -> action", getName());

			_state = true;
			_actionCb(this, true);
		}
	}
	else
	{
		if (_state)
		{
			FB_DEBUG_LOG_I_OBJ("[%s] condition NOT met -> action", getName());

			_state = false;
			_actionCb(this, false);
		}
	}
}

void SwitchIface::setForseFlag(SwitchForseState state)
{
	_forseFlag = state;
}

bool SwitchIface::isOn() const
{
	return _state;
}
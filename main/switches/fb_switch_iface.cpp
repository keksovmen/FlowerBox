#include "fb_switch_iface.hpp"



using namespace fb;
using namespace switches;



SwitchIface::SwitchIface(const ConditionCb &conditionCb, const ActionCb actionCb)
	: _conditionCb(conditionCb), _actionCb(actionCb)
{
}

SwitchIface::~SwitchIface()
{

}

void SwitchIface::check()
{
	if (_conditionCb(this))
	{
		if (!_state)
		{
			FB_DEBUG_LOG("[%s] condition met -> action", getName());

			_state = true;
			_actionCb(this, true);
		}
	}
	else
	{
		if (_state)
		{
			FB_DEBUG_LOG("[%s] condition NOT met -> action", getName());

			_state = false;
			_actionCb(this, false);
		}
	}
}

bool SwitchIface::isOn() const
{
	return _state;
}
#include "fb_switch_service.hpp"



using namespace fb;
using namespace switches;



const char* SwitchService::getName()
{
	return "SwitchService";
}

void SwitchService::addSwitch(SwitchIface* entry)
{
	//TODO: TAKE MUTEX
	_entries.push_back(entry);
}

void SwitchService::_onPull()
{
	for(SwitchIface* s : _entries){
		s->check();
	}
}
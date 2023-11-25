#include "fb_state_booted.hpp"

#include "fb_state_wifi_connect.hpp"
#include "fb_wifi.hpp"



using namespace fb;
using namespace state;



StateBooted::StateBooted(StateManager& context)
	: State(context)
{

}

const char* StateBooted::getName()
{
	return "StateBooted";
}

void StateBooted::handleEvent(const event::Event& event)
{
	if(event.groupId == event::EventGroup::BOOT){
		getContext().transition(std::unique_ptr<State>(new StateWifiConnect(getContext())));
	}
}

void StateBooted::enter()
{
	//инициализируем единоразовыи компоненты
	wifi::init();
}

void StateBooted::exit()
{
	//все оставляем как есть
}

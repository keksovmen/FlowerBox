#include "fb_state_booted.hpp"

#include "fb_clock.hpp"
#include "fb_state_initialized.hpp"
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

void StateBooted::enter()
{
	//инициализируем единоразовыи компоненты
	wifi::init();
	//достаем откуда либо настройки для подключения к сети
	//подключаемся к сети вифи, причем ждем успешного соединения
	//TODO: вернуть смог или нет спустя сколько то попыток
	wifi::connect(CONFIG_EXAMPLE_WIFI_SSID, CONFIG_EXAMPLE_WIFI_PASSWORD);
	//инициализируем sntp и время
	//синхронизируем время
	if(!clock::syncTime()){
		//TODO: не смогли, чо делать?
	}

	//переходим в рабочий режим
	getContext().transition(std::unique_ptr<State>(new StateInitialized(getContext())));
}

void StateBooted::exit()
{
	//все оставляем как есть
}

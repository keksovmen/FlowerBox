#include "fb_globals.hpp"

#include <memory>

#ifdef _ESP8266
	#include "esp_system.h"
#else
	#include "esp_efuse.h"
	#include "esp_efuse_table.h"
#endif
#include "esp_system.h"

#include "fb_core.hpp"
#include "fb_nvs_storage.hpp"
#include "fb_ram_storage.hpp"
#include "fb_sensor_store_service.hpp"
#include "fb_settings.hpp"
#include "fb_time_switch.hpp"



#define _DEVICE_NAME "FlowerBox"



using namespace fb;
using namespace global;



static const char* TAG = "globals";



//сервисы туть
static event::EventManager _eventManager;
static state::StateManager _stateManager("STATE_MANAGER");



//TODO: made it somehow not call new in constructor, because it leads to crashes
static sensor::SensorStoreService _sensorStoreService(project::getHwSensorStorage());
static util::TimeScheduler _timeScheduler;



static void _init_settings()
{
	std::unique_ptr<storage::StorageIface> store(std::make_unique<storage::NvsStorage>());
	if(!store->init()){
		FB_DEBUG_LOG_I_TAG("Failed to init NVS storage -> RAM will be used");

		store.reset(new storage::RamStorage());
		store->init();
	}

	settings::init(std::move(store));
}



void global::init()
{
	_init_settings();

	_timeScheduler.start();

	project::initHwObjs();
	project::initBoxObjs();
	project::initMaperObjs();

	_eventManager.attachListener(&_sensorStoreService);
	_eventManager.attachListener(&project::getInfo().core);
}

void global::restart()
{
	getTimeScheduler()->addActionDelayed([](){esp_restart();}, 5000, portMAX_DELAY);
}

event::EventManager* global::getEventManager()
{
	return &_eventManager;
}

state::StateManager* global::getStateManager()
{
	return &_stateManager;
}

id::UniqueId global::getUniqueId()
{
	uint8_t mac[8] = {0};
	#ifdef _ESP8266
		esp_read_mac(mac, ESP_MAC_WIFI_STA);
	#else
		ESP_ERROR_CHECK(esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, &mac, 6 * 8));
	#endif

	return *reinterpret_cast<id::UniqueId*>(mac);
}

box::Box* global::getFlowerBox()
{
	return &project::getBox();
}

sensor::SensorService* global::getSensorService()
{
	return &project::getHwSensorService();
}

sensor::SensorStorage* global::getSensorStorage()
{
	return &project::getHwSensorStorage();
}

sensor::SensorStoreService* global::getSensorStoreService()
{
	return &_sensorStoreService;
}

switches::SwitchService* global::getSwitchService()
{
	return &project::getHwSwitchService();
}

std::string global::getDeviceName()
{
	return _DEVICE_NAME "_" + std::to_string(global::getUniqueId());
}

util::TimeScheduler* global::getTimeScheduler()
{
	return &_timeScheduler;
}
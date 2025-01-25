#include "fb_globals.hpp"

#include <memory>

#include "esp_efuse.h"
#include "esp_efuse_table.h"

#include "fb_main_state_manager.hpp"
#include "fb_sensor_store_service.hpp"
#include "fb_heat_switch.hpp"
#include "fb_time_switch.hpp"
#include "fb_nvs_storage.hpp"
#include "fb_ram_storage.hpp"
#include "fb_settings.hpp"
#include "fb_project_box_obj.hpp"
#include "fb_project_hw_obj.hpp"
#include "fb_project_maper_obj.hpp"



#define _DEVICE_NAME "FlowerBox"



using namespace fb;
using namespace global;



static const char* TAG = "globals";



//сервисы туть
static event::EventManager _eventManager;
static pins::PinManager _pinManager;
static state::StateManager _stateManager("STATE_MANAGER");



//TODO: made it somehow not call new in constructor, because it leads to crashes
static sensor::SensorStoreService _sensorStoreService(project::getHwSensorStorage());



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

	project::initHwObjs();
	project::initBoxObjs();
	project::initMaperObjs();

	_eventManager.attachListener(&_sensorStoreService);
}

event::EventManager* global::getEventManager()
{
	return &_eventManager;
}

pins::PinManager* global::getPinManager()
{
	return &_pinManager;
}

state::StateManager* global::getStateManager()
{
	return &_stateManager;
}

id::UniqueId global::getUniqueId()
{
	uint8_t mac[8] = {0};
    ESP_ERROR_CHECK(esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, &mac, 6 * 8));

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

switches::SwitchService* global::getSwitchService()
{
	return &project::getHwSwitchService();
}

std::string global::getDeviceName()
{
	return _DEVICE_NAME "_" + std::to_string(global::getUniqueId());
}
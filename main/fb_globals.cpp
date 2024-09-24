#include "fb_globals.hpp"

#include <memory>

#include "esp_efuse.h"
#include "esp_efuse_table.h"

#include "fb_main_state_manager.hpp"
#include "fb_box_service.hpp"
#include "fb_heat_switch.hpp"
#include "fb_time_switch.hpp"



using namespace fb;
using namespace global;



//сервисы туть
static event::EventManager _eventManager;
static pins::PinManager _pinManager;
static state::StateManager _stateManager("STATE_MANAGER");

static sensor::SensorService _sensorService;
static sensor::SensorStorage _sensorStorage;

static switches::SwitchService _swithService;

static box::Box _flowerBox("TEST_NAME", "0.0.1", getUniqueId());
//TODO: made it somehow not call new in constructor, because it leads to crashes
static box::BoxService* _boxService;

//сенсоры туть
static sensor::TempreatureSensorTest _sensorTemperature(pins::PIN_SENSOR_TEMPERATURE, 2);

//переключатели туть
static switches::TimeSwitch _switchLight(clock::Time(0, 0, 0), clock::Time(0, 1, 0), pins::PIN_BLUE_LED);
static switches::HeatSwitch _switchHeating(&_sensorTemperature, 0, 28.5, 29, pins::PIN_GREEN_LED);
static switches::FanSwitch _switchFan(&_sensorTemperature, 1, 30, 31, pins::PIN_COOL_LED);



static void _registerSwitchProperties(switches::SwitchIface* sw, box::Tid tid)
{
	auto* forseProperty = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_FORSE,
		[sw](int val){
			sw->setForseFlag(static_cast<bool>(val));
			return true;
		}, sw->isOn()
	);
	//TODO: possible fuck up?
	_flowerBox.addProperty(std::unique_ptr<box::PropertyIface>(forseProperty));

	auto* switchBoxWrapper = new box::Switch(
		tid,
		{forseProperty->getId()},
		{},
		[sw](){return sw->isOn();}
	);
	_flowerBox.addSwitch(switchBoxWrapper);
}



void global::init()
{
	_boxService = new box::BoxService(_flowerBox, _sensorStorage);

	_sensorService.addSensor(&_sensorTemperature);

	_swithService.addSwitch(&_switchLight);
	_swithService.addSwitch(&_switchHeating);
	_swithService.addSwitch(&_switchFan);

	//TODO: put somewhere else, in to BoxService and let it listen for events -> create properties and devices
	_registerSwitchProperties(&_switchLight, box::Tid::SWITCH_LIGHT);
	_registerSwitchProperties(&_switchHeating, box::Tid::SWITCH_HEAT);
	_registerSwitchProperties(&_switchFan, box::Tid::SWITCH_FAN);

	auto* swapProperty = new box::PropertyInt(box::Tid::PROPERTY_SWAP_TEMP_SENSOR_INDEX,
		[](int val){
			_switchHeating.setSensorIndex(_switchHeating.getSensorIndex() == 0 ? 1 : 0);	
			_switchFan.setSensorIndex(_switchFan.getSensorIndex() == 0 ? 1 : 0);	
			return true;
		},
		0);
	_flowerBox.addProperty(std::unique_ptr<box::PropertyIface>(swapProperty));
	

	_eventManager.attachListener(_boxService);
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
	return &_flowerBox;
}

sensor::SensorService* global::getSensorService()
{
	return &_sensorService;
}

sensor::SensorStorage* global::getSensorStorage()
{
	return &_sensorStorage;
}

switches::SwitchService* global::getSwitchService()
{
	return &_swithService;
}
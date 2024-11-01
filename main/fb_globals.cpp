#include "fb_globals.hpp"

#include <memory>

#include "esp_efuse.h"
#include "esp_efuse_table.h"

#include "fb_main_state_manager.hpp"
#include "fb_box_service.hpp"
#include "fb_heat_switch.hpp"
#include "fb_time_switch.hpp"



#define _DEVICE_NAME "FlowerBox"



using namespace fb;
using namespace global;



//сервисы туть
static event::EventManager _eventManager;
static pins::PinManager _pinManager;
static state::StateManager _stateManager("STATE_MANAGER");

static sensor::SensorService _sensorService;
static sensor::SensorStorage _sensorStorage;

static switches::SwitchService _swithService;

static box::Box _flowerBox(_DEVICE_NAME, "0.0.1", getUniqueId());
//TODO: made it somehow not call new in constructor, because it leads to crashes
static box::BoxService* _boxService;

//сенсоры туть
static sensor::TemperatureSensorArray<2> _sensorArrayTemp(pins::PIN_SENSOR_TEMPERATURE);
static sensor::TemperatureSensor* _sensorTemperatureInside = _sensorArrayTemp.getSensor(0);
static sensor::TemperatureSensor* _sensorTemperatureOutside = _sensorArrayTemp.getSensor(1);

//переключатели туть
static switches::TimeSwitch _switchLight(clock::Time(0, 0, 0), clock::Time(0, 1, 0), pins::PIN_BLUE_LED);
static switches::HeatSwitch _switchHeating(_sensorTemperatureInside, 28.5, 29, pins::PIN_GREEN_LED);
static switches::FanSwitch _switchFan(_sensorTemperatureOutside, 30, 31, pins::PIN_COOL_LED);

static box::Sensor* _boxTempSensors[2] = {nullptr, nullptr};
static box::Switch* _boxLightSwitch = nullptr;
static box::Switch* _boxHeatSwitch = nullptr;
static box::Switch* _boxFanSwitch = nullptr;



static box::Switch* _registerSwitchProperties(switches::SwitchIface* sw, box::Tid tid)
{
	auto* forseProperty = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_FORSE,
		[sw](int val){
			sw->setForseFlag(static_cast<switches::SwitchForseState>(val));
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

	return switchBoxWrapper;
}

static void _registerSensor()
{
	for(int i = 0; i < _sensorArrayTemp.getDeviceCount(); i++)
	{
		auto* sen = new box::Sensor(box::Tid::SENSOR_DS18B20);
		_flowerBox.addSensor(sen);
		
		//добвить свойства: период опроса, название датчика
		const auto* prop = _flowerBox.addProperty(std::make_unique<box::PropertyString>(
			"Temp. sensor " + std::to_string(sen->getId()) + " description",
			"Defines sensor description with id " + std::to_string(sen->getId()),
			box::Tid::PROPERTY_SENSOR_DESCRIPTION,
			[](std::string val){return true;},
			sen->getDescription()));
		
		sen->addPropertyDependency(prop->getId());

		_boxTempSensors[i] = sen;
	}
}

static void _setBoxSwitchSensors()
{
	_boxHeatSwitch->addSensorDependency(_boxTempSensors[0]->getId());
	_boxFanSwitch->addSensorDependency(_boxTempSensors[1]->getId());
}


void global::init()
{
	_boxService = new box::BoxService(_flowerBox, _sensorStorage);

	_sensorService.addSensor(&_sensorArrayTemp);

	_swithService.addSwitch(&_switchLight);
	_swithService.addSwitch(&_switchHeating);
	_swithService.addSwitch(&_switchFan);

	_registerSensor();

	//TODO: put somewhere else, in to BoxService and let it listen for events -> create properties and devices
	_boxLightSwitch = _registerSwitchProperties(&_switchLight, box::Tid::SWITCH_LIGHT);
	_boxHeatSwitch = _registerSwitchProperties(&_switchHeating, box::Tid::SWITCH_HEAT);
	_boxFanSwitch = _registerSwitchProperties(&_switchFan, box::Tid::SWITCH_FAN);


	_setBoxSwitchSensors();

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

std::string global::getDeviceName()
{
	return _DEVICE_NAME "_" + std::to_string(global::getUniqueId());
}
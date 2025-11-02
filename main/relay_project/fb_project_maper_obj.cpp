#include "fb_project_maper_obj.hpp"

#include <ranges>

#include "fb_globals.hpp"
#include "fb_project_box_obj.hpp"
#include "fb_project_hw_obj.hpp"
#include "fb_settings.hpp"
#include "fb_switch.hpp"



using namespace fb;
using namespace project;



static box::Switch _gpioSwitch(box::Tid::SWITCH_GPIO_ARRAY,
	[](){return getHwGpioSwitch().isOn(0);});

static box::Switch _hhtpPullerSwitch(box::Tid::SWITCH_HTTP_PULLER,
	[](){return getHwHttpPuller().isWorking();});


static void _create_and_register_forse_property(switches::SwitchIface& obj, box::Switch& dependy)
{
	auto* forseProperty = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_FORSE,
		[&obj](int val){
			obj.setForseFlag(static_cast<switches::SwitchForseState>(val));

			getHwSwitchService().forcePullAction();

			return true;
		}, obj.isOn()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(forseProperty));
	dependy.addPropertyDependency(forseProperty->getId());
}



static void _init_box_properties()
{
	auto* settingsProp = new box::PropertyNone(box::Tid::PROPERTY_SYSTEM_RESET_SETTINGS,
		[](auto val){
			settings::clearWifi();
			return true;
		});
	getBox().addProperty(std::unique_ptr<box::PropertyIface>(settingsProp));
	getBox().addPropertyDependency(settingsProp->getId());


	auto* restartProp = new box::PropertyNone(box::Tid::PROPERTY_SYSTEM_RESTART,
		[](auto val){
			global::getTimeScheduler()->addActionDelayed([](){esp_restart();}, 5000, portMAX_DELAY);
			return true;
		});
	getBox().addProperty(std::unique_ptr<box::PropertyIface>(restartProp));
	getBox().addPropertyDependency(restartProp->getId());
}


static void _initGpioArray()
{
	_create_and_register_forse_property(getHwGpioSwitch(), _gpioSwitch);

	auto* turnOnProperty = new box::PropertyInt("TurnOn", "Turns specific GPIO on",
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			getHwGpioSwitch().turnOn(val);
			return true;
		}, 0, 0, 6
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(turnOnProperty));
	_gpioSwitch.addPropertyDependency(turnOnProperty->getId());


	auto* turnOffProperty = new box::PropertyInt("TurnOff", "Turns specific GPIO off",
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			getHwGpioSwitch().turnOff(val);
			return true;
		}, 0, 0, 6
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(turnOffProperty));
	_gpioSwitch.addPropertyDependency(turnOffProperty->getId());



	getBox().addSwitch(&_gpioSwitch);
}

static void _initHttpPuller()
{
	auto* tuggleProperty = new box::PropertyInt("TurnOn/Off", "",
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			getHwHttpPuller().setPause(!static_cast<bool>(val));
			return true;
		}, getHwHttpPuller().isWorking(), 0, 1
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(tuggleProperty));
	_hhtpPullerSwitch.addPropertyDependency(tuggleProperty->getId());

	getBox().addSwitch(&_hhtpPullerSwitch);
}



void project::initMaperObjs()
{
	_init_box_properties();
	_initGpioArray();
	_initHttpPuller();
}

int project::mapBoxSensorIdToAddres(int id)
{
	// if(id == getBoxMp3Sensor().getId()){
	// 	return reinterpret_cast<int>(&getHwMp3Sensor());
	// }

	assert(0);
}

int project::mapBoxSwitchIdToAddres(int id)
{
	if(id == _gpioSwitch.getId())
	{
		return reinterpret_cast<int>(&_gpioSwitch);
	}

	assert(0);
}
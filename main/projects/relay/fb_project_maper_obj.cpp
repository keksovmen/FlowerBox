#include "fb_core.hpp"

#include <ranges>

#include "fb_globals.hpp"
#include "fb_relay_hw_obj.hpp"
#include "fb_relay_settings.hpp"
#include "fb_settings.hpp"
#include "fb_switch.hpp"



using namespace fb;
using namespace project;



static box::Switch _gpioSwitch(box::Tid::SWITCH_GPIO_ARRAY,
	[](){return getHwGpioSwitch().isOn(0);});

static box::Switch _hhtpPullerSwitch(box::Tid::SWITCH_HTTP_PULLER,
	[](){return getHwHttpPuller().isWorking();});



static void _initGpioArray()
{
	fb::project::util::createAndRegisterForceProperty(getHwGpioSwitch(), _gpioSwitch);

	auto* turnOnProperty = new box::PropertyInt("TurnOn", "Turns specific GPIO on",
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			getHwGpioSwitch().turnOn(val);
			return true;
		}, 0, 0, pins::RELAY_PINS_COUNT
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(turnOnProperty));
	_gpioSwitch.addPropertyDependency(turnOnProperty->getId());


	auto* turnOffProperty = new box::PropertyInt("TurnOff", "Turns specific GPIO off",
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			getHwGpioSwitch().turnOff(val);
			return true;
		}, 0, 0, pins::RELAY_PINS_COUNT
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(turnOffProperty));
	_gpioSwitch.addPropertyDependency(turnOffProperty->getId());



	getBox().addSwitch(&_gpioSwitch);
}

static void _initHttpPuller()
{
	auto* toggleProperty = new box::PropertyInt("TurnOn/Off", "",
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			getHwHttpPuller().setPause(!static_cast<bool>(val));
			return true;
		}, getHwHttpPuller().isWorking(), 0, 1
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(toggleProperty));
	_hhtpPullerSwitch.addPropertyDependency(toggleProperty->getId());


	auto* delayProperty = new box::PropertyInt("Delay ms", "How long to wait for next fetch after successive attempt",
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			getHwHttpPuller().setTimeoutMs(val);
			settings::setHttpDelay(val);
			return true;
		}, getHwHttpPuller().getTimeoutMs(), 0, 10000
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(delayProperty));
	_hhtpPullerSwitch.addPropertyDependency(delayProperty->getId());


	auto* urlProperty = new box::PropertyString("URL", "Where to call",
		box::Tid::PROPERTY_GENERAL,
		[](std::string val){
			getHwHttpPuller().setUrl(val);
			settings::setHttpUrl(val);
			return true;
		}, std::string(getHwHttpPuller().getUrl())
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(urlProperty));
	_hhtpPullerSwitch.addPropertyDependency(urlProperty->getId());


	getBox().addSwitch(&_hhtpPullerSwitch);
}



void project::initMaperObjs()
{
	util::createAndRegisterDefaultBoxProperties();
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
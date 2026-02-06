#include "fb_core.hpp"

#include "fb_globals.hpp"
#include "fb_keypad_backlight_box_obj.hpp"
#include "fb_keypad_backlight_hw_obj.hpp"
#include "fb_keypad_backlight_settings.hpp"
#include "fb_switch.hpp"



using namespace fb;
using namespace project;



static void _registerDb135Properties()
{
	auto* valProp = new box::PropertyInt(
		"DB135", "Diods driver, each bit of value is 1 = activate, 0 = disable pin, max 16 pin",
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			setDbState(val);
			return true;
		},
		getDbState(), 0, (1 << 16) - 1);
	//made it dynamic
	valProp->setGetAction([](){return getHwWrapperDb().getValue();});

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(valProp));
	getBox().addPropertyDependency(valProp->getId());


	auto* pinPulseProp = new box::PropertyInt(
		"Pulse pin", "Start PIN in pulse mode",
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			setPulseMode(val);
			return true;
		},
		0, 0, 15);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(pinPulseProp));
	getBox().addPropertyDependency(pinPulseProp->getId());


	auto* pulseTimeProp = new box::PropertyInt("PulseTime", "Change PulseTime",
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setPulseTime(val);
			return true;
		}, settings::getPulseTime(), 1, 10 
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(pulseTimeProp));
	getBox().addPropertyDependency(pulseTimeProp->getId());
}

static void _initMqtt()
{
	auto* mqttId = new box::PropertyInt("Mqtt ID", "Will apply after reboot",
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setMqttId(val);
			return true;
		}, settings::getMqttId(), 0, 255
	);
	getBox().addProperty(std::unique_ptr<box::PropertyIface>(mqttId));
	getBox().addPropertyDependency(mqttId->getId());

	auto* mqttIp = new box::PropertyString("Mqtt IP", "Will apply after reboot",
		box::Tid::PROPERTY_GENERAL,
		[](std::string val){
			settings::setIp(val);
			return true;
		}, settings::getIp()
	);
	getBox().addProperty(std::unique_ptr<box::PropertyIface>(mqttIp));
	getBox().addPropertyDependency(mqttIp->getId());

	auto* mqttPort = new box::PropertyInt("Mqtt port", "Will apply after reboot",
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setPort(val);
			return true;
		}, settings::getPort(), 0, (1 << 16) - 1
	);
	getBox().addProperty(std::unique_ptr<box::PropertyIface>(mqttPort));
	getBox().addPropertyDependency(mqttPort->getId());
}


void project::initMaperObjs()
{
	util::createAndRegisterDefaultBoxProperties();
	_registerDb135Properties();
	_initMqtt();
	
}

int project::mapBoxSensorIdToAddres(int id)
{
	// if(id == getBoxInsideTempSensor().getId())
	// {
	// 	return reinterpret_cast<int>(&getHwAhtSensor());
	// }

	//TODO: add proper error returning

	assert(0);
}

int project::mapBoxSwitchIdToAddres(int id)
{
	// if(id == _boxLightSwitch.getId())
	// {
	// 	return reinterpret_cast<int>(&_boxLightSwitch);
	// }

	assert(0);
}
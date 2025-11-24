#include "fb_core_maper_obj.hpp"

#include "fb_core_box_obj.hpp"
#include "fb_core_hw_obj.hpp"
#include "fb_globals.hpp"
#include "fb_property.hpp"
#include "fb_settings.hpp"



using namespace fb;
using namespace project;



void fb::project::util::createAndRegisterForceProperty(switches::SwitchIface& obj, box::Switch& dependy)
{
	auto* forseProperty = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_FORSE,
		[&obj](int val){
			obj.setForseFlag(static_cast<switches::SwitchForseState>(val));
			//to act immediatly not on next tick
			getHwSwitchService().forcePullAction();
			
			return true;
		}, obj.isOn()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(forseProperty));
	dependy.addPropertyDependency(forseProperty->getId());
}

void fb::project::util::createAndRegisterDefaultBoxProperties()
{
	const auto* sensorServiceProp = getBox().addProperty(std::make_unique<box::PropertyInt>(
		box::Tid::PROPERTY_SENSOR_PERIOD_GLOBAL,
		[](int val){
			getHwSensorService().setTimerPeriod(val * 1000);
			return true;
		},
		3));	//TODO: made it not die in assert
		// getHwSensorService().getTimerPeriod()));
	getBox().addPropertyDependency(sensorServiceProp->getId());


	//TODO: add dynamic value updates, as java beans
	auto* currentTimeProp = new box::PropertyInt(
		box::Tid::PROPERTY_SYSTEM_TIME,
		[](int millis){

			return false;
		},
		clock::currentTimeStamp());
	//made it dynamic
	currentTimeProp->setGetAction([](){return clock::currentTimeStamp();});

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(currentTimeProp));
	getBox().addPropertyDependency(currentTimeProp->getId());



	auto* settingsProp = new box::PropertyNone(box::Tid::PROPERTY_SYSTEM_RESET_SETTINGS,
		[](auto val){
			settings::clearWifi();
			return true;
		});
	getBox().addProperty(std::unique_ptr<box::PropertyIface>(settingsProp));
	getBox().addPropertyDependency(settingsProp->getId());


	auto* restartProp = new box::PropertyNone(box::Tid::PROPERTY_SYSTEM_RESTART,
		[](auto val){
			global::restart();
			return true;
		});
	getBox().addProperty(std::unique_ptr<box::PropertyIface>(restartProp));
	getBox().addPropertyDependency(restartProp->getId());



	auto* wifiSsidProperty = new box::PropertyString("Wifi SSID", "Change wifi ssid",
		box::Tid::PROPERTY_GENERAL,
		[](std::string val){
			settings::setStaSsid(val);
			return true;
		}, settings::getStaSsid()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(wifiSsidProperty));
	getBox().addPropertyDependency(wifiSsidProperty->getId());


	auto* wifiPassProperty = new box::PropertyString("Wifi pass", "Change wifi password",
		box::Tid::PROPERTY_GENERAL,
		[](std::string val){
			settings::setStaPass(val);
			return true;
		}, settings::getStaPass()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(wifiPassProperty));
	getBox().addPropertyDependency(wifiPassProperty->getId());
}

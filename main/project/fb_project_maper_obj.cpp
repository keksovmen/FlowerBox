#include "fb_project_maper_obj.hpp"

#include "fb_project_box_obj.hpp"
#include "fb_project_hw_obj.hpp"
#include "fb_switch.hpp"



using namespace fb;
using namespace project;



static box::Switch _boxLightSwitch(box::Tid::SWITCH_LIGHT,
	[](){return getHwLightSwitch().isOn();});

static box::Switch _boxHeatSwitch(box::Tid::SWITCH_LIGHT,
	[](){return getHwHeatSwitch().isOn();});

static box::Switch _boxFanSwitch(box::Tid::SWITCH_LIGHT,
	[](){return getHwFanSwitch().isOn();});



void project::initMaperObjs()
{
	getBox().addSwitch(&_boxLightSwitch);
	getBox().addSwitch(&_boxHeatSwitch);
	getBox().addSwitch(&_boxFanSwitch);


	auto* forseProperty = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_FORSE,
		[](int val){
			getHwLightSwitch().setForseFlag(static_cast<switches::SwitchForseState>(val));
			return true;
		}, getHwLightSwitch().isOn()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(forseProperty));
	_boxLightSwitch.addPropertyDependency(forseProperty->getId());


	forseProperty = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_FORSE,
		[](int val){
			getHwHeatSwitch().setForseFlag(static_cast<switches::SwitchForseState>(val));
			return true;
		}, getHwHeatSwitch().isOn()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(forseProperty));
	_boxHeatSwitch.addPropertyDependency(forseProperty->getId());
	_boxHeatSwitch.addSensorDependency(getBoxInsideSensor().getId());


	forseProperty = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_FORSE,
		[](int val){
			getHwFanSwitch().setForseFlag(static_cast<switches::SwitchForseState>(val));
			return true;
		}, getHwFanSwitch().isOn()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(forseProperty));
	_boxFanSwitch.addPropertyDependency(forseProperty->getId());
	_boxFanSwitch.addSensorDependency(getBoxInsideSensor().getId());


	getBox().addProperty(std::make_unique<box::PropertyInt>(
		box::Tid::PROPERTY_SENSOR_PERIOD_GLOBAL,
		[](int val){
			getHwSensorService().setTimerPeriod(val * 1000);
			return true;
		},
		3));	//TODO: made it not die in assert
		// getHwSensorService().getTimerPeriod()));
		

	//TODO: add sensor service property and etc
}
#include "fb_project_maper_obj.hpp"

#include "fb_globals.hpp"
#include "fb_project_box_obj.hpp"
#include "fb_project_hw_obj.hpp"
#include "fb_project_settings.hpp"
#include "fb_settings.hpp"
#include "fb_switch.hpp"



using namespace fb;
using namespace project;



static box::Switch _boxLightSwitch(box::Tid::SWITCH_LIGHT,
	[](){return getHwLightSwitch().isOn();});

static box::Switch _boxHeatSwitch(box::Tid::SWITCH_HEAT,
	[](){return getHwHeatSwitch().isOn();});

static box::Switch _boxFanSwitch(box::Tid::SWITCH_FAN,
	[](){return getHwFanSwitch().isOn();});



static void _create_and_register_forse_property(switches::SwitchIface& obj, box::Switch& dependy)
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

static void _init_light_switch()
{
	_create_and_register_forse_property(getHwLightSwitch(), _boxLightSwitch);


	auto* startTimeProp = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_LIGHT_ON,
		[](int val){
			getHwLightSwitch().setStartTime(val);
			getHwHeatSwitch().setDayStartTime(val);
			getHwFanSwitch().setDayStartTime(val);
			//TODO: maybe put store in to swith iface somehow
			//maybe chain of responsibility or composite
			settings::setLightStartTime(val);

			return true;
		},
		static_cast<int>(getHwLightSwitch().getStartTime())
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(startTimeProp));
	_boxLightSwitch.addPropertyDependency(startTimeProp->getId());


	auto* endTimeProp = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_LIGHT_OFF,
		[](int val){
			getHwLightSwitch().setEndTime(val);
			getHwHeatSwitch().setDayEndTime(val);
			getHwFanSwitch().setDayEndTime(val);
			settings::setLightEndTime(val);

			return true;
		},
		static_cast<int>(getHwLightSwitch().getEndTime())
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(endTimeProp));
	_boxLightSwitch.addPropertyDependency(endTimeProp->getId());
}

static void _init_heat_switch()
{
	_create_and_register_forse_property(getHwHeatSwitch(), _boxHeatSwitch);
	_boxHeatSwitch.addSensorDependency(getBoxInsideTempSensor().getId());


	auto* lowTempProperty = new box::PropertyFloat(box::Tid::PROPERTY_SWITCH_LOW_TEMP,
		[](float val){
			getHwHeatSwitch().setLowValue(val);
			//TODO: maybe put store in to swith iface somehow
			//maybe chain of responsibility or composite
			settings::setHeaterLowTemp(val);

			return true;
		},
		getHwHeatSwitch().getLowValue()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(lowTempProperty));
	_boxHeatSwitch.addPropertyDependency(lowTempProperty->getId());


	auto* highTempProperty = new box::PropertyFloat(box::Tid::PROPERTY_SWITCH_HIGH_TEMP,
		[](float val){
			getHwHeatSwitch().setHighValue(val);
			//TODO: maybe put store in to swith iface somehow
			//maybe chain of responsibility or composite
			settings::setHeaterHighTemp(val);

			return true;
		},
		getHwHeatSwitch().getHighValue()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(highTempProperty));
	_boxHeatSwitch.addPropertyDependency(highTempProperty->getId());


	auto* deltaTempProperty = new box::PropertyFloat(box::Tid::PROPERTY_SWITCH_DELTA_TEMP,
		[](float val){
			getHwHeatSwitch().setDelta(val);
			//TODO: maybe put store in to swith iface somehow
			//maybe chain of responsibility or composite
			settings::setHeaterDayNightDelta(val);

			return true;
		},
		getHwHeatSwitch().getDelta()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(deltaTempProperty));
	_boxHeatSwitch.addPropertyDependency(deltaTempProperty->getId());
}

static void _init_fan_switch()
{
	_create_and_register_forse_property(getHwFanSwitch(), _boxFanSwitch);
	_boxFanSwitch.addSensorDependency(getBoxInsideTempSensor().getId());

	auto* speedPwmProperty = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_SPEED,
		[](int val){
			getHwFanSwitch().setSpeed(val);
			settings::setFanSpeed(val);

			return true;
		},
		getHwFanSwitch().getSpeed()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(speedPwmProperty));
	_boxFanSwitch.addPropertyDependency(speedPwmProperty->getId());


	auto* lowTempProperty = new box::PropertyFloat(box::Tid::PROPERTY_SWITCH_LOW_TEMP,
		[](float val){
			getHwFanSwitch().setTempLowValue(val);
			//TODO: maybe put store in to swith iface somehow
			//maybe chain of responsibility or composite
			settings::setFanLowTemp(val);

			return true;
		},
		getHwFanSwitch().getTempLowValue()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(lowTempProperty));
	_boxFanSwitch.addPropertyDependency(lowTempProperty->getId());


	auto* highTempProperty = new box::PropertyFloat(box::Tid::PROPERTY_SWITCH_HIGH_TEMP,
		[](float val){
			getHwFanSwitch().setTempHighValue(val);
			//TODO: maybe put store in to swith iface somehow
			//maybe chain of responsibility or composite
			settings::setFanHighTemp(val);

			return true;
		},
		getHwFanSwitch().getTempHighValue()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(highTempProperty));
	_boxFanSwitch.addPropertyDependency(highTempProperty->getId());


	auto* lowHumProperty = new box::PropertyFloat(box::Tid::PROPERTY_SWITCH_LOW_HUMIDITY,
		[](float val){
			getHwFanSwitch().setHumLowValue(val);
			//TODO: maybe put store in to swith iface somehow
			//maybe chain of responsibility or composite
			settings::setFanLowHum(val);

			return true;
		},
		getHwFanSwitch().getHumLowValue()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(lowHumProperty));
	_boxFanSwitch.addPropertyDependency(lowHumProperty->getId());


	auto* highHumProperty = new box::PropertyFloat(box::Tid::PROPERTY_SWITCH_HIGH_HUMIDITY,
		[](float val){
			getHwFanSwitch().setHumHighValue(val);
			//TODO: maybe put store in to swith iface somehow
			//maybe chain of responsibility or composite
			settings::setFanHighHum(val);

			return true;
		},
		getHwFanSwitch().getHumHighValue()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(highHumProperty));
	_boxFanSwitch.addPropertyDependency(highHumProperty->getId());


	auto* deltaHumProperty = new box::PropertyFloat(box::Tid::PROPERTY_SWITCH_DELTA_HUMIDITY,
		[](float val){
			getHwFanSwitch().setDelta(val);
			//TODO: maybe put store in to swith iface somehow
			//maybe chain of responsibility or composite
			settings::setFanDayNightDelta(val);

			return true;
		},
		getHwFanSwitch().getDelta()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(deltaHumProperty));
	_boxFanSwitch.addPropertyDependency(deltaHumProperty->getId());
}

static void _init_box_properties()
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
			global::getTimeScheduler()->addActionDelayed([](){esp_restart();}, 5000, portMAX_DELAY);
			return true;
		});
	getBox().addProperty(std::unique_ptr<box::PropertyIface>(restartProp));
	getBox().addPropertyDependency(restartProp->getId());
}



void project::initMaperObjs()
{
	getBox().addSwitch(&_boxLightSwitch);
	getBox().addSwitch(&_boxHeatSwitch);
	getBox().addSwitch(&_boxFanSwitch);


	_init_light_switch();
	_init_heat_switch();
	_init_fan_switch();

	_init_box_properties();
}

int project::mapBoxSensorIdToAddres(int id)
{
	if(id == getBoxInsideTempSensor().getId())
	{
		return reinterpret_cast<int>(&getHwAhtSensor());
	}
	else if(id == getBoxOutsideTempSensor().getId())
	{
		return reinterpret_cast<int>(&getHwDS18Sensor());
	}
	else if(id == getBoxInsideHumiditySensor().getId())
	{
		//TODO: comment offset
		return reinterpret_cast<int>(&getHwAhtSensor()) + 1;
	}

	//TODO: add proper error returning

	assert(0);
	// switch(id)
	// {
		//try constexpr here
	// 	case getBoxInsideTempSensor().getId():

	// }
}

int project::mapBoxSwitchIdToAddres(int id)
{
	if(id == _boxLightSwitch.getId())
	{
		return reinterpret_cast<int>(&_boxLightSwitch);
	}
	else if(id == _boxHeatSwitch.getId())
	{
		return reinterpret_cast<int>(&_boxHeatSwitch);
	}
	else if(id == _boxFanSwitch.getId())
	{
		return reinterpret_cast<int>(&_boxFanSwitch);
	}

	assert(0);
}

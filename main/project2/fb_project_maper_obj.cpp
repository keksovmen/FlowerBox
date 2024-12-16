#include "fb_project_maper_obj.hpp"

#include <ranges>

#include "fb_project_box_obj.hpp"
#include "fb_project_hw_obj.hpp"
#include "fb_switch.hpp"



using namespace fb;
using namespace project;



static box::Switch _boxRgbSwitch(box::Tid::SWITCH_RGB,
	[](){return getHwRgbSwitch().isOn();});

// static box::Switch _boxHeatSwitch(box::Tid::SWITCH_HEAT,
// 	[](){return getHwHeatSwitch().isOn();});

// static box::Switch _boxFanSwitch(box::Tid::SWITCH_FAN,
// 	[](){return getHwFanSwitch().isOn();});



static void _create_and_register_forse_property(switches::SwitchIface& obj, box::Switch& dependy)
{
	auto* forseProperty = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_FORSE,
		[&obj](int val){
			obj.setForseFlag(static_cast<switches::SwitchForseState>(val));
			return true;
		}, obj.isOn()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(forseProperty));
	dependy.addPropertyDependency(forseProperty->getId());
}



static void _initRgbSwitch()
{
	_create_and_register_forse_property(getHwRgbSwitch(), _boxRgbSwitch);
	auto* colorProperty = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_RGB_VALUE,
		[](int val){
			getHwRgbSwitch().setColor(val);
			return true;
		}, getHwRgbSwitch().getColor()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(colorProperty));
	_boxRgbSwitch.addPropertyDependency(colorProperty->getId());
}

static void _initMp3Sensor()
{
	//must be write only
	auto* playProperty = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_MP3_PLAY,
		[](int val){
			return getHwMp3Sensor().play(val);
		}, 0
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(playProperty));
	getBoxMp3Sensor().addPropertyDependency(playProperty->getId());


	auto* stopProperty = new box::PropertyNone(
		box::Tid::PROPERTY_SWITCH_MP3_STOP,
		[](std::string val){
			getHwMp3Sensor().stop();
			return true;
		});

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(stopProperty));
	getBoxMp3Sensor().addPropertyDependency(stopProperty->getId());


	auto* volumeProperty = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_MP3_VOLUME,
		[](int val){
			return getHwMp3Sensor().setVolume(val);
		}, getHwMp3Sensor().getVolume()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(volumeProperty));
	getBoxMp3Sensor().addPropertyDependency(volumeProperty->getId());
}

void project::initMaperObjs()
{
	getBox().addSwitch(&_boxRgbSwitch);
	// getBox().addSwitch(&_boxLightSwitch);
	// getBox().addSwitch(&_boxHeatSwitch);


	_initRgbSwitch();
	_initMp3Sensor();

	// _create_and_register_forse_property(getHwHeatSwitch(), _boxHeatSwitch);
	// _boxHeatSwitch.addSensorDependency(getBoxInsideSensor().getId());

	// _create_and_register_forse_property(getHwFanSwitch(), _boxFanSwitch);
	// _boxFanSwitch.addSensorDependency(getBoxInsideSensor().getId());


	// const auto* prop = getBox().addProperty(std::make_unique<box::PropertyInt>(
	// 	box::Tid::PROPERTY_SENSOR_PERIOD_GLOBAL,
	// 	[](int val){
	// 		getHwSensorService().setTimerPeriod(val * 1000);
	// 		return true;
	// 	},
	// 	3));	//TODO: made it not die in assert
	// 	// getHwSensorService().getTimerPeriod()));

	// const auto* prop = getBox().addProperty(std::make_unique<box::PropertyInt>(
	// 	box::Tid::PROPERTY_SENSOR_PERIOD_GLOBAL,
	// 	[](int val){
	// 		getHwSensorService().setTimerPeriod(val * 1000);
	// 		return true;
	// 	},
	// 	3));	//TODO: made it not die in assert
	// getBox().addPropertyDependency(prop->getId());
		

	//TODO: add sensor service property and etc
}

int project::mapBoxSensorIdToAddres(int id)
{
	// if(id == getBoxInsideSensor().getId())
	// {
	// 	return reinterpret_cast<int>(&getHwInsideTempSensor());
	// }
	// else if(id == getBoxOutsideSensor().getId())
	// {
	// 	return reinterpret_cast<int>(&getHwOutsideTempSensor());
	// }

	//TODO: add proper error returning

	assert(0);
	// switch(id)
	// {
		//try constexpr here
	// 	case getBoxInsideSensor().getId():

	// }
}

int project::mapBoxSwitchIdToAddres(int id)
{
	if(id == _boxRgbSwitch.getId())
	{
		return reinterpret_cast<int>(&_boxRgbSwitch);
	}
	// else if(id == _boxHeatSwitch.getId())
	// {
	// 	return reinterpret_cast<int>(&_boxHeatSwitch);
	// }
	// else if(id == _boxFanSwitch.getId())
	// {
	// 	return reinterpret_cast<int>(&_boxFanSwitch);
	// }

	assert(0);
}

box::PropertyIface& project::getRgbProperty()
{
	// auto res = std::ranges::find_if(
	// 	_boxRgbSwitch.getPropertyDependencies(),
	// 	[](const auto& val){return val.getTid() == box::Tid::PROPERTY_SWITCH_RGB_VALUE;});
	
	// assert(res != _boxRgbSwitch.getPropertyDependencies().end());

	// return *res;
	return *getBox().getProperty(_boxRgbSwitch.getPropertyDependencies().at(1));
}

box::PropertyIface& project::getRgbSwitchProperty()
{
	return *getBox().getProperty(_boxRgbSwitch.getPropertyDependencies().at(0));
}
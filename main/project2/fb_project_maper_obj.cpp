#include "fb_project_maper_obj.hpp"

#include <ranges>

#include "fb_globals.hpp"
#include "fb_project_box_obj.hpp"
#include "fb_project_hw_obj.hpp"
#include "fb_settings.hpp"
#include "fb_switch.hpp"



using namespace fb;
using namespace project;



static box::Switch _boxRgbSwitch(box::Tid::SWITCH_RGB,
	[](){return getHwRgbSwitch().isOn();});



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



void project::initMaperObjs()
{
	getBox().addSwitch(&_boxRgbSwitch);


	_initRgbSwitch();
	_initMp3Sensor();
	_init_box_properties();
}

int project::mapBoxSensorIdToAddres(int id)
{
	if(id == getBoxMp3Sensor().getId()){
		return reinterpret_cast<int>(&getHwMp3Sensor());
	}

	assert(0);
}

int project::mapBoxSwitchIdToAddres(int id)
{
	if(id == _boxRgbSwitch.getId())
	{
		return reinterpret_cast<int>(&_boxRgbSwitch);
	}

	assert(0);
}

box::PropertyIface& project::getRgbProperty()
{
	return *getBox().getProperty(_boxRgbSwitch.getPropertyDependencies().at(1));
}

box::PropertyIface& project::getRgbSwitchProperty()
{
	return *getBox().getProperty(_boxRgbSwitch.getPropertyDependencies().at(0));
}

box::PropertyIface& project::getMp3PlayProperty()
{
	return *getBox().getProperty(getBoxMp3Sensor().getPropertyDependencies().at(0));
}

box::PropertyIface& project::getMp3StopProperty()
{
	return *getBox().getProperty(getBoxMp3Sensor().getPropertyDependencies().at(1));
}

box::PropertyIface& project::getMp3ValueProperty()
{
	return *getBox().getProperty(getBoxMp3Sensor().getPropertyDependencies().at(2));
}

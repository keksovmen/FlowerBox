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
			getHwWrapperDb().setValue(val);
			return true;
		},
		getHwWrapperDb().getValue(), 0, (1 << 16) - 1);
	//made it dynamic
	valProp->setGetAction([](){return getHwWrapperDb().getValue();});

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(valProp));
	getBox().addPropertyDependency(valProp->getId());
}



void project::initMaperObjs()
{
	util::createAndRegisterDefaultBoxProperties();
	_registerDb135Properties();
	
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
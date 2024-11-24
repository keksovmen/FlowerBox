#include "fb_project_maper_obj.hpp"

#include "fb_project_box_obj.hpp"
#include "fb_project_hw_obj.hpp"
#include "fb_switch.hpp"



using namespace fb;
using namespace project;



// static box::Switch _boxLightSwitch(box::Tid::SWITCH_LIGHT,
// 	[](){return getHwLightSwitch().isOn();});

// static box::Switch _boxHeatSwitch(box::Tid::SWITCH_HEAT,
// 	[](){return getHwHeatSwitch().isOn();});

// static box::Switch _boxFanSwitch(box::Tid::SWITCH_FAN,
// 	[](){return getHwFanSwitch().isOn();});



// static void _create_and_register_forse_property(switches::SwitchIface& obj, box::Switch& dependy)
// {
// 	auto* forseProperty = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_FORSE,
// 		[&obj](int val){
// 			obj.setForseFlag(static_cast<switches::SwitchForseState>(val));
// 			return true;
// 		}, obj.isOn()
// 	);

// 	getBox().addProperty(std::unique_ptr<box::PropertyIface>(forseProperty));
// 	dependy.addPropertyDependency(forseProperty->getId());
// }



void project::initMaperObjs()
{
	// getBox().addSwitch(&_boxLightSwitch);
	// getBox().addSwitch(&_boxHeatSwitch);
	// getBox().addSwitch(&_boxFanSwitch);


	// _create_and_register_forse_property(getHwLightSwitch(), _boxLightSwitch);

	// _create_and_register_forse_property(getHwHeatSwitch(), _boxHeatSwitch);
	// _boxHeatSwitch.addSensorDependency(getBoxInsideSensor().getId());

	// _create_and_register_forse_property(getHwFanSwitch(), _boxFanSwitch);
	// _boxFanSwitch.addSensorDependency(getBoxInsideSensor().getId());


	const auto* prop = getBox().addProperty(std::make_unique<box::PropertyInt>(
		box::Tid::PROPERTY_SENSOR_PERIOD_GLOBAL,
		[](int val){
			getHwSensorService().setTimerPeriod(val * 1000);
			return true;
		},
		3));	//TODO: made it not die in assert
		// getHwSensorService().getTimerPeriod()));
	getBox().addPropertyDependency(prop->getId());
		

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
	// if(id == _boxLightSwitch.getId())
	// {
	// 	return reinterpret_cast<int>(&_boxLightSwitch);
	// }
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

#include "fb_project_box_obj.hpp"

#include "fb_globals.hpp"
#include "fb_property.hpp"



#define _DEVICE_NAME "FlowerBox"
#define _DEVICE_VERSION "0.0.1"



using namespace fb;
using namespace project;



static box::Box _flowerBox(_DEVICE_NAME, _DEVICE_VERSION, global::getUniqueId());
// static box::Sensor _insideSensor(box::Tid::SENSOR_DS18B20);
// static box::Sensor _outsideSensor(box::Tid::SENSOR_DS18B20);



// static void _create_register_description_prop(box::ObjectIface& obj)
// {
// 	const auto *prop = _flowerBox.addProperty(std::make_unique<box::PropertyString>(
// 		box::Tid::PROPERTY_SENSOR_DESCRIPTION,
// 		[&obj](std::string val)
// 		{ 
// 			obj.setDescription(val);
// 			return true;
// 		},
// 		obj.getDescription())
// 	);
// 	assert(prop);

// 	obj.addPropertyDependency(prop->getId());
// }


void project::initBoxObjs()
{
	//change to box get sensor id???
	// _flowerBox.addSensor(&_insideSensor);
	// _flowerBox.addSensor(&_outsideSensor);

	// change to static somehow
	// _create_register_description_prop(_insideSensor);
	// _create_register_description_prop(_outsideSensor);
}

box::Box& project::getBox()
{
	return _flowerBox;	
}

// box::Sensor& project::getBoxInsideSensor()
// {
// 	return _insideSensor;
// }

// box::Sensor& project::getBoxOutsideSensor()
// {
// 	return _outsideSensor;
// }

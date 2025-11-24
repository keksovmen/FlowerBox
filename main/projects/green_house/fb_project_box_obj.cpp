#include "fb_green_house_box_obj.hpp"

#include "fb_globals.hpp"
#include "fb_property.hpp"



#define _DEVICE_NAME "FlowerBox"
#define _DEVICE_VERSION "0.0.1"



using namespace fb;
using namespace project;



static box::Box _flowerBox(_DEVICE_NAME, _DEVICE_VERSION, global::getUniqueId());
static box::Sensor _insideTempSensor(box::Tid::SENSOR_TEMPERATURE);
static box::Sensor _insideHumiditySensor(box::Tid::SENSOR_HUMIDITY);
static box::Sensor _outsideTempSensor(box::Tid::SENSOR_TEMPERATURE);



static void _create_register_description_prop(box::ObjectIface& obj)
{
	const auto *prop = _flowerBox.addProperty(std::make_unique<box::PropertyString>(
		box::Tid::PROPERTY_SENSOR_DESCRIPTION,
		[&obj](std::string val)
		{ 
			obj.setDescription(val);
			return true;
		},
		obj.getDescription())
	);
	assert(prop);

	obj.addPropertyDependency(prop->getId());
}


void project::initBoxObjs()
{
	//change to box get sensor id???
	_flowerBox.addSensor(&_insideTempSensor);
	_flowerBox.addSensor(&_outsideTempSensor);
	_flowerBox.addSensor(&_insideHumiditySensor);

	//change descriptions
	_insideTempSensor.setDescription("Inside temperature sensor");
	_insideHumiditySensor.setDescription("Inside humidity sensor");
	_outsideTempSensor.setDescription("Outside temperature sensor");

	// change to static somehow
	_create_register_description_prop(_insideTempSensor);
	_create_register_description_prop(_outsideTempSensor);
	_create_register_description_prop(_insideHumiditySensor);
}

box::Box& project::getBox()
{
	return _flowerBox;	
}

box::Sensor& project::getBoxInsideTempSensor()
{
	return _insideTempSensor;
}

box::Sensor& project::getBoxInsideHumiditySensor()
{
	return _insideHumiditySensor;
}

box::Sensor& project::getBoxOutsideTempSensor()
{
	return _outsideTempSensor;
}

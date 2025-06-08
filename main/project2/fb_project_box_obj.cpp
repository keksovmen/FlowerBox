#include "fb_project_box_obj.hpp"

#include "fb_globals.hpp"
#include "fb_property.hpp"



#define _DEVICE_NAME "Устройство квест Мафия"
#define _DEVICE_VERSION "0.0.1"



using namespace fb;
using namespace project;



static box::Box _flowerBox(_DEVICE_NAME, _DEVICE_VERSION, global::getUniqueId());

static box::Sensor _boxMp3Sensor(box::Tid::SENSOR_MP3);



void project::initBoxObjs()
{
	getBox().addSensor(&getBoxMp3Sensor());
}

box::Box& project::getBox()
{
	return _flowerBox;	
}

box::Sensor& project::getBoxMp3Sensor()
{
	return _boxMp3Sensor;
}
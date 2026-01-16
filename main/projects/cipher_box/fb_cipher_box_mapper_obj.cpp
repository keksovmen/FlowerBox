#include "fb_core.hpp"

#include "fb_globals.hpp"
#include "fb_cipher_box_box_obj.hpp"
#include "fb_cipher_box_hw_obj.hpp"
#include "fb_cipher_box_settings.hpp"



using namespace fb;
using namespace project;



void project::initMaperObjs()
{
	util::createAndRegisterDefaultBoxProperties();
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
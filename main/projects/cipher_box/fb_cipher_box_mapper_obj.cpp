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

	
	
	auto* UrlProp = new box::PropertyString("Url", "Change Url",
    
		box::Tid::PROPERTY_GENERAL,
		[](std::string val){
			settings::setUrl(val);
			return true;
		}, settings::getUrl()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(UrlProp));
	getBox().addPropertyDependency(UrlProp->getId());
	
	
	auto* DeltaErrorProp = new box::PropertyInt("DeltaError", "Change DeltaError",
	
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setDeltaError(val);
			return true;
		}, settings::getDeltaError(), 0, 100 
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(DeltaErrorProp));
	getBox().addPropertyDependency(DeltaErrorProp->getId());
	
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
#include "fb_core.hpp"

#include "fb_globals.hpp"
#include "fb_rfids_box_obj.hpp"
#include "fb_rfids_hw_obj.hpp"
#include "fb_rfids_settings.hpp"



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
	
	
	auto* LogProp = new box::PropertyInt("Log", "Change Log",
	
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setLog(val);
			return true;
		}, settings::getLog(), 0, 1 
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(LogProp));
	getBox().addPropertyDependency(LogProp->getId());
	
	
	auto* ReadTimeMsProp = new box::PropertyInt("ReadTimeMs", "Change ReadTimeMs",
	
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setReadTimeMs(val);
			return true;
		}, settings::getReadTimeMs(), 100, 300 
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(ReadTimeMsProp));
	getBox().addPropertyDependency(ReadTimeMsProp->getId());
	
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
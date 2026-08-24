#include "fb_core.hpp"

#include "fb_globals.hpp"
#include "fb_washer_box_obj.hpp"
#include "fb_washer_hw_obj.hpp"
#include "fb_washer_settings.hpp"



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
	
	
	auto* BounceTimeMsProp = new box::PropertyInt("BounceTimeMs", "Change BounceTimeMs",
	
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setBounceTimeMs(val);
			return true;
		}, settings::getBounceTimeMs(), 0, 500 
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(BounceTimeMsProp));
	getBox().addPropertyDependency(BounceTimeMsProp->getId());
	
	
	auto* MqttIdProp = new box::PropertyInt("MqttId", "Change MqttId",
	
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setMqttId(val);
			return true;
		}, settings::getMqttId(), 0, 4096 
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(MqttIdProp));
	getBox().addPropertyDependency(MqttIdProp->getId());
	
	
	auto* IpProp = new box::PropertyString("Ip", "Change Ip",
    
		box::Tid::PROPERTY_GENERAL,
		[](std::string val){
			settings::setIp(val);
			return true;
		}, settings::getIp()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(IpProp));
	getBox().addPropertyDependency(IpProp->getId());
	
	
	auto* PortProp = new box::PropertyInt("Port", "Change Port",
	
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setPort(val);
			return true;
		}, settings::getPort(), 0, 65535 
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(PortProp));
	getBox().addPropertyDependency(PortProp->getId());
	
	
	auto* BlinkPeriodMsProp = new box::PropertyInt("BlinkPeriodMs", "Change BlinkPeriodMs",
	
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setBlinkPeriodMs(val);
			return true;
		}, settings::getBlinkPeriodMs(), 0, 65535 
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(BlinkPeriodMsProp));
	getBox().addPropertyDependency(BlinkPeriodMsProp->getId());
	
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
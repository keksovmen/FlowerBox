#include "fb_core.hpp"

#include "fb_globals.hpp"
#include "fb_relay_box_obj.hpp"
#include "fb_relay_hw_obj.hpp"
#include "fb_relay_settings.hpp"



using namespace fb;
using namespace project;



void project::initMaperObjs()
{
	util::createAndRegisterDefaultBoxProperties();

	
	
	auto* HttpUrlProp = new box::PropertyString("HttpUrl", "Change HttpUrl",
    
		box::Tid::PROPERTY_GENERAL,
		[](std::string val){
			settings::setHttpUrl(val);
			return true;
		}, settings::getHttpUrl()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(HttpUrlProp));
	getBox().addPropertyDependency(HttpUrlProp->getId());
	
	
	auto* HttpDelayProp = new box::PropertyInt("HttpDelay", "Change HttpDelay",
	
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setHttpDelay(val);
			return true;
		}, settings::getHttpDelay(), 0, 10000 
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(HttpDelayProp));
	getBox().addPropertyDependency(HttpDelayProp->getId());
	
	
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
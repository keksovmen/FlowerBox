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
	
	
	auto* BounceTimeMsProp = new box::PropertyInt("BounceTimeMs", "Change BounceTimeMs",
	
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setBounceTimeMs(val);
			return true;
		}, settings::getBounceTimeMs(), 0, 10000 
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(BounceTimeMsProp));
	getBox().addPropertyDependency(BounceTimeMsProp->getId());
	
	
	auto* ChipsCountProp = new box::PropertyInt("ChipsCount", "Change ChipsCount",
	
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setChipsCount(val);
			return true;
		}, settings::getChipsCount(), 1, 3 
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(ChipsCountProp));
	getBox().addPropertyDependency(ChipsCountProp->getId());
	
	
	auto* ReadingsCountProp = new box::PropertyInt("ReadingsCount", "Change ReadingsCount",
	
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setReadingsCount(val);
			return true;
		}, settings::getReadingsCount(), 1, 16 
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(ReadingsCountProp));
	getBox().addPropertyDependency(ReadingsCountProp->getId());
	
	
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
	
	
	auto* UseHttpProp = new box::PropertyInt("UseHttp", "Change UseHttp",
	
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setUseHttp(val);
			return true;
		}, settings::getUseHttp(), 0, 1 
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(UseHttpProp));
	getBox().addPropertyDependency(UseHttpProp->getId());
	
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
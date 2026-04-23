#include "fb_core.hpp"

#include "fb_globals.hpp"
#include "fb_input_matrix_box_obj.hpp"
#include "fb_input_matrix_hw_obj.hpp"
#include "fb_input_matrix_settings.hpp"



using namespace fb;
using namespace project;



void project::initMaperObjs()
{
	util::createAndRegisterDefaultBoxProperties();

	
	
	auto* MqttIdProp = new box::PropertyInt("MqttId", "Change MqttId",
	
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setMqttId(val);
			return true;
		}, settings::getMqttId(), 0, 255 
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
	
	
	auto* PulseTimeProp = new box::PropertyInt("PulseTime", "Change PulseTime",
	
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setPulseTime(val);
			return true;
		}, settings::getPulseTime(), 1, 10 
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(PulseTimeProp));
	getBox().addPropertyDependency(PulseTimeProp->getId());
	
	
	auto* CombinationProp = new box::PropertyString("Combination", "Change Combination",
    
		box::Tid::PROPERTY_GENERAL,
		[](std::string val){
			settings::setCombination(val);
			return true;
		}, settings::getCombination()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(CombinationProp));
	getBox().addPropertyDependency(CombinationProp->getId());
	
	
	auto* DebounceMsProp = new box::PropertyInt("DebounceMs", "Change DebounceMs",
	
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setDebounceMs(val);
			return true;
		}, settings::getDebounceMs(), 0, 10000 
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(DebounceMsProp));
	getBox().addPropertyDependency(DebounceMsProp->getId());
	
	
	auto* StillPeriodMsProp = new box::PropertyInt("StillPeriodMs", "Change StillPeriodMs",
	
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setStillPeriodMs(val);
			return true;
		}, settings::getStillPeriodMs(), 0, 100000 
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(StillPeriodMsProp));
	getBox().addPropertyDependency(StillPeriodMsProp->getId());
	
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
#include "fb_core.hpp"

#include "fb_globals.hpp"
#include "fb_tressure_box_obj.hpp"
#include "fb_tressure_hw_obj.hpp"
#include "fb_tressure_settings.hpp"



using namespace fb;
using namespace project;



//TODO: duplicate same as dmx_light project
static void _initMqtt()
{
	auto* mqttId = new box::PropertyInt("Mqtt ID", "Will apply after reboot",
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			settings::setMqttId(val);
			return true;
		}, settings::getMqttId(), 0, 255
	);
	getBox().addProperty(std::unique_ptr<box::PropertyIface>(mqttId));
	getBox().addPropertyDependency(mqttId->getId());

	auto* mqttIp = new box::PropertyString("Mqtt IP", "Will apply after reboot",
		box::Tid::PROPERTY_GENERAL,
		[](std::string val){
			settings::setIp(val);
			return true;
		}, settings::getIp()
	);
	getBox().addProperty(std::unique_ptr<box::PropertyIface>(mqttIp));
	getBox().addPropertyDependency(mqttIp->getId());

	auto* mqttPort = new box::PropertyInt("Mqtt port", "Will apply after reboot",
		box::Tid::PROPERTY_GENERAL,
		[](int val){
			// getHwMqttClient().setPause(!static_cast<bool>(val));
			settings::setPort(val);
			return true;
		}, settings::getPort(), 0, (1 << 16) - 1
	);
	getBox().addProperty(std::unique_ptr<box::PropertyIface>(mqttPort));
	getBox().addPropertyDependency(mqttPort->getId());
}



void project::initMaperObjs()
{
	util::createAndRegisterDefaultBoxProperties();
	_initMqtt();

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
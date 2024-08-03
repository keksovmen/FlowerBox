#include "fb_state_sensor_init.hpp"

#include "fb_sensor_service.hpp"



using namespace fb;
using namespace state;



StateSensorInit::StateSensorInit(StateManager& context)
	: State(context)
{

}

void StateSensorInit::enter()
{
	// sensor::SensorService service(10);
	// service.start();
	// for(;;){
		// vTaskDelay(pdMS_TO_TICKS(1000));
		// service.forseScan();
		// service.forseRead();
		// vTaskDelay(pdMS_TO_TICKS(1000));
		// for(const auto& v : service.getSensors())
		// {
		// 	FB_DEBUG_LOG("Sensor[%s]: %llX, value = %.2f", v.alive ? "+" : "-", v.id, v.value)
		// }
	// }
}

void StateSensorInit::exit()
{

}

void StateSensorInit::handleEvent(const event::Event& event)
{

}

const char* StateSensorInit::getName()
{
	return "StateSensorInit";
}

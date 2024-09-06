#include "fb_sensor_iface.hpp"



using namespace fb;
using namespace sensor;



SensorIface::~SensorIface()
{
}

bool SensorIface::init()
{
	const bool result = _doInit();
	FB_DEBUG_LOG("init() = %d", result);
	_initFlag = true;

	return result;
}

bool SensorIface::update()
{
	const bool result = _doUpdate();
	FB_DEBUG_LOG("update() = %d", result);

	if(!result){
		_initFlag = false;
	}

	return result;
}

bool SensorIface::isInit() const
{
	return _initFlag;
}

#include "fb_sensor_iface.hpp"

#include <utility>



using namespace fb;
using namespace sensor;



bool SensorIface::init()
{
	const bool result = _doInit();
	FB_DEBUG_LOG_I_OBJ("init() = %d", result);
	_initFlag = result;

	return result;
}

bool SensorIface::update()
{
	const UpdateResult result = _doUpdate();
	FB_DEBUG_LOG_I_OBJ("update() = %d", std::to_underlying(result));

	switch(result)
	{
		case UpdateResult::OK:
			return false;
		
		case UpdateResult::VALUE_CHANGED:
			return true;
		
		case UpdateResult::FAIL:
		{
			_initFlag = false;
			return false;
		}

		default:
			assert(0);
			return false;
	}
}

bool SensorIface::isInit() const
{
	return _initFlag;
}

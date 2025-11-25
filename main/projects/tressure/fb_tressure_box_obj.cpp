#include "fb_tressure_box_obj.hpp"

#include "fb_globals.hpp"



#define _DEVICE_NAME ""
#define _DEVICE_VERSION ""



using namespace fb;
using namespace project;



static box::Box _flowerBox(_DEVICE_NAME, _DEVICE_VERSION, global::getUniqueId());



void project::initBoxObjs()
{

}

box::Box& project::getBox()
{
	return _flowerBox;	
}
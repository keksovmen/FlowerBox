#pragma once



#include "fb_core.hpp"
#include "fb_property.hpp"



namespace fb
{
	namespace project
	{
		box::PropertyIface& getRgbProperty();
		box::PropertyIface& getRgbSwitchProperty();

		box::PropertyIface& getMp3PlayProperty();
		box::PropertyIface& getMp3StopProperty();
		box::PropertyIface& getMp3ValueProperty();
	}
}
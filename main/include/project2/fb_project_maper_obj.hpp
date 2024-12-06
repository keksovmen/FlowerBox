#pragma once



#include "fb_property.hpp"



namespace fb
{
	namespace project
	{
		void initMaperObjs();
		int mapBoxSensorIdToAddres(int id);
		int mapBoxSwitchIdToAddres(int id);

		box::PropertyIface& getRgbProperty();
		box::PropertyIface& getRgbSwitchProperty();
	}
}
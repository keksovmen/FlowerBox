#pragma once



#include "fb_switch_iface.hpp"
#include "fb_switch.hpp"



namespace fb
{
	namespace project
	{
		void initMaperObjs();
		//TODO: need no address but unique id in HW system
		int mapBoxSensorIdToAddres(int id);
		//TODO: need no address but unique id in HW system
		int mapBoxSwitchIdToAddres(int id);

		namespace util
		{
			void createAndRegisterForceProperty(switches::SwitchIface& obj, box::Switch& dependy);
			void createAndRegisterDefaultBoxProperties();
		}
	}
}
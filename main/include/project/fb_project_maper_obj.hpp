#pragma once



namespace fb
{
	namespace project
	{
		void initMaperObjs();
		//TODO: need no address but unique id in HW system
		int mapBoxSensorIdToAddres(int id);
		//TODO: need no address but unique id in HW system
		int mapBoxSwitchIdToAddres(int id);
	}
}
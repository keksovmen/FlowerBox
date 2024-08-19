#pragma once



#include <string>



namespace fb
{
	namespace box
	{
		//TODO: add more tids
		enum class Tid : int
		{
			SENSOR_DS18B20 = 0,

			PROPERTY_SENSOR_PERIOD_GLOBAL = 512,
			PROPERTY_SENSOR_DESCRIPTION,

			MAX
		};

		int operatir(const Tid& t);
		


		const char* tidToName(Tid tid);
		const char* tidToDescription(Tid tid);
		const char* tidToValueType(Tid tid);
		const char* tidToMinValue(Tid tid);
		const char* tidToMaxValue(Tid tid);
	}
}



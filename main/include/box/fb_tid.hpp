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

			SWITCH_LIGHT = 256,
			SWITCH_HEAT,
			SWITCH_FAN,
			SWITCH_RGB,
			SWITCH_MP3,

			PROPERTY_SENSOR_PERIOD_GLOBAL = 512,
			PROPERTY_SENSOR_DESCRIPTION,
			PROPERTY_SWITCH_FORSE,

			PROPERTY_SWITCH_RGB_VALUE,
			PROPERTY_SWITCH_MP3_PLAY,
			PROPERTY_SWITCH_MP3_STOP,

			MAX
		};



		namespace priv
		{
			struct _TidHolder
			{
				const char* name;
				const char* description;
				const char* valueType;
				const char* minValue;
				const char* maxValue;

				// constexpr _TidHolder(
				// 	const char* name,
				// 	const char* description,
				// 	const char* valueType,
				// 	const char* minValue,
				// 	const char* maxValue
				// ) : name(name), description(description), valueType(valueType), minValue(minValue), maxValue(maxValue) {}
				// constexpr ~_TidHolder() = default;
			};

			constexpr const _TidHolder map(Tid tid){
				switch(tid)
				{
					case Tid::SENSOR_DS18B20:
						return {"DS18B20", "Temperature sensor", "float", "-100.0", "100.0"};
						// return _TidHolder("DS18B20", "Temperature sensor", "float", "-100.0", "100.0");

					case Tid::SWITCH_LIGHT:
						return {"Light switch", "", "bool", "0", "1"};
						// return _TidHolder("Light switch", "", "bool", "0", "1");

					case Tid::SWITCH_HEAT:
						return {"Heat switch", "", "bool", "0", "1"};
						// return _TidHolder("Heat switch", "", "bool", "0", "1");

					case Tid::SWITCH_FAN:
						return {"Fun switch", "", "bool", "0", "1"};
						// return _TidHolder("Fun switch", "", "bool", "0", "1");

					case Tid::SWITCH_RGB:
						return {"RGB switch", "", "bool", "0", "1"};

					case Tid::SWITCH_MP3:
						return {"Mp3 player switch", "", "bool", "0", "1"};

					case Tid::PROPERTY_SENSOR_PERIOD_GLOBAL:
						return {"Sensor mesure period", "measured in SEC", "int", "1", "600"};
						// return _TidHolder("Sensor mesure period", "measured in SEC", "int", "1", "600");

					case Tid::PROPERTY_SENSOR_DESCRIPTION:
						return {"Changes description", "", "str", "0", "300"};
						// return _TidHolder("Changes description", "", "str", "0", "300");

					case Tid::PROPERTY_SWITCH_FORSE:
						return {"Forses state", "", "int", "0", "2"};
						// return _TidHolder("Forses state", "", "int", "0", "2");

					case Tid::PROPERTY_SWITCH_RGB_VALUE:
						return {"RGB", "Send in decimals not hex", "int", "0", "16777215"};

					case Tid::PROPERTY_SWITCH_MP3_PLAY:
						return {"Mp3 play", "Accepts track id and starts to play", "int", "0", "65535"};

					case Tid::PROPERTY_SWITCH_MP3_STOP:
						return {"Mp3 stop", "Stop playing", "none", "0", "0"};

					default:
						// abort();
						return {"FIX_ME", "FIX_ME", "FIX_ME", "FIX_ME", "FIX_ME"};
				}
			}
		};



		constexpr const char* tidToName(Tid tid){return priv::map(tid).name;};
		constexpr const char* tidToDescription(Tid tid){return priv::map(tid).description;};
		constexpr const char* tidToValueType(Tid tid){return priv::map(tid).valueType;};
		constexpr const char* tidToMinValue(Tid tid){return priv::map(tid).minValue;};
		constexpr const char* tidToMaxValue(Tid tid){return priv::map(tid).maxValue;};
	}
}



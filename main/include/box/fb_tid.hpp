#pragma once



#include <string>



namespace fb
{
	namespace box
	{
		//TODO: add more tids
		enum class Tid : int
		{
			SENSOR_MP3 = 0,
			SENSOR_TEMPERATURE,
			SENSOR_HUMIDITY,

			SWITCH_LIGHT = 256,
			SWITCH_HEAT,
			SWITCH_FAN,
			SWITCH_RGB,

			PROPERTY_SENSOR_PERIOD_GLOBAL = 512,
			PROPERTY_SENSOR_DESCRIPTION,
			PROPERTY_SWITCH_FORSE,

			PROPERTY_SWITCH_RGB_VALUE,
			PROPERTY_SWITCH_MP3_PLAY,
			PROPERTY_SWITCH_MP3_STOP,
			PROPERTY_SWITCH_MP3_VOLUME,

			PROPERTY_SWITCH_LIGHT_ON,
			PROPERTY_SWITCH_LIGHT_OFF,

			PROPERTY_SYSTEM_TIME,

			PROPERTY_SYSTEM_RESET_SETTINGS,
			PROPERTY_SYSTEM_RESTART,

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
			};

			constexpr const _TidHolder map(Tid tid){
				switch(tid)
				{
					case Tid::SENSOR_TEMPERATURE:
						return {"Temperature", "General temperature sensor", "float", "-40.0", "100.0"};
					
					case Tid::SENSOR_HUMIDITY:
						return {"Humidity", "General humidity sensor", "float", "0.0", "100.0"};

					case Tid::SENSOR_MP3:
						return {"Mp3 player", "", "int", "-1", "2000"};

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

					case Tid::PROPERTY_SWITCH_MP3_VOLUME:
						return {"Mp3 volume", "Set volume", "int", "0", "30"};

					case Tid::PROPERTY_SWITCH_LIGHT_ON:
						return {"Start time", "At what time to turn ON, in seconds", "int", "0", "86399"};

					case Tid::PROPERTY_SWITCH_LIGHT_OFF:
						return {"End time", "At what time to turn OFF, in seconds", "int", "0", "86399"};

					case Tid::PROPERTY_SYSTEM_TIME:
						return {"Current system time", "Unix time since 1970", "int", "0", "4294967295"};

					case Tid::PROPERTY_SYSTEM_RESET_SETTINGS:
						return {"Reset settings", "Deletes all WIFI settings", "none", "0", "0"};

					case Tid::PROPERTY_SYSTEM_RESTART:
						return {"System restart", "Restarts the system", "none", "0", "0"};

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



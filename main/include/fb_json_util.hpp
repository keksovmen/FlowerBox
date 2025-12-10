#pragma once



#include <string_view>

#include <cJSON.h>



namespace fb
{
	namespace json_util
	{
		struct RGB
		{
			int r = 0;
			int g = 0;
			int b = 0;
		};



		int getIntFromJsonOrDefault(cJSON* json, std::string_view key, int def)
		{
			auto* obj = cJSON_GetObjectItem(json, key.cbegin());
			if(obj != nullptr){
				return static_cast<int>(cJSON_GetNumberValue(obj));
			}

			return def;
		}

		RGB parseRgbFromJson(std::string_view data)
		{
			cJSON* obj = cJSON_ParseWithLength(data.begin(), data.length());
			const int r = getIntFromJsonOrDefault(obj, "R", 0);
			const int g = getIntFromJsonOrDefault(obj, "G", 0);
			const int b = getIntFromJsonOrDefault(obj, "B", 0);

			cJSON_Delete(obj);

			return {r, g, b};
		}		
	}
}
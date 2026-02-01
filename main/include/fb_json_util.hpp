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

		bool parseNumberArray(std::string_view jsonStr, std::string_view arrayName,
                    std::function<void(int)> elementCallback)
		{
			const char* TAG = "JSON";

			cJSON* root = cJSON_Parse(jsonStr.begin());
			if (root == nullptr) {
				FB_DEBUG_LOG_E_TAG("Error parsing JSON: %s", jsonStr.begin());
				return false;
			}
			
			cJSON* jsonArray = cJSON_GetObjectItem(root, arrayName.begin());
			if (jsonArray == nullptr) {
				FB_DEBUG_LOG_E_TAG("Error getting obj: %s, from %s", arrayName.begin(), arrayName.begin());
				cJSON_Delete(root);
				return false;
			}
			
			if (!cJSON_IsArray(jsonArray)) {
				FB_DEBUG_LOG_E_TAG("IS not an array: %s, %s", arrayName.begin(), arrayName.begin());
				cJSON_Delete(root);
				return false;
			}
			
			cJSON* arrayItem = nullptr;
			cJSON_ArrayForEach(arrayItem, jsonArray) {
				if (cJSON_IsNumber(arrayItem)) {
					int value = arrayItem->valueint;
					std::invoke(elementCallback, value);
				} else if (cJSON_IsString(arrayItem)) {
					FB_DEBUG_LOG_W_TAG("String value found, skipping");
				} else {
					FB_DEBUG_LOG_W_TAG("Unsupported type in array, skipping");
				}
			}
			
			cJSON_Delete(root);
			return true;
		}
	}
}
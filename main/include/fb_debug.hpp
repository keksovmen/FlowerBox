#pragma once

#include "esp_log.h"



#define FB_DEBUG_TAG_ENTER(){ESP_LOGI(TAG, "Entered: %s", __FUNCTION__);}
#define FB_DEBUG_TAG_EXIT(){ESP_LOGI(TAG, "Exited: %s", __FUNCTION__);}

#define FB_DEBUG_ENTER(){ESP_LOGI(getName(), "Entered: %s", __FUNCTION__);}
#define FB_DEBUG_EXIT(){ESP_LOGI(getName(), "Exited: %s", __FUNCTION__);}

#define FB_DEBUG_TAG_LOG(format, ...){ESP_LOGI(TAG, format, ##__VA_ARGS__);}
#define FB_DEBUG_TAG_LOG_W(format, ...){ESP_LOGW(TAG, format, ##__VA_ARGS__);}
#define FB_DEBUG_TAG_LOG_E(format, ...){ESP_LOGE(TAG, format, ##__VA_ARGS__);}
#define FB_DEBUG_LOG(format, ...){ESP_LOGI(getName(), format, ##__VA_ARGS__);}
#define FB_DEBUG_PTR_LOG(ptr, format, ...){ESP_LOGI((ptr)->getName(), format, ##__VA_ARGS__);}



namespace fb
{
	namespace debug
	{
		class Named
		{
			public:
				//TODO: make it const
				virtual const char* getName() const = 0;
		};
	};
}
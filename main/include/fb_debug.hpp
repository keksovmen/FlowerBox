#pragma once

#include "esp_log.h"



#define FB_DEBUG_ENTER(){ESP_LOGI(getName(), "Entered: %s", __FUNCTION__);}
#define FB_DEBUG_EXIT(){ESP_LOGI(getName(), "Exited: %s", __FUNCTION__);}



namespace fb
{
	namespace debug
	{
		class Named
		{
			public:
				virtual const char* getName() = 0;
		};
	};
}
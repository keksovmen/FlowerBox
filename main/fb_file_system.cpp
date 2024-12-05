#include "fb_file_system.hpp"

#include "esp_spiffs.h"

#include "fb_debug.hpp"



#define _PARTITION_NAME "storage"



using namespace fb;
using namespace fs;



static const char* TAG = "fb_file_system";



void fs::init()
{
	FB_DEBUG_ENTER_I_TAG();

	esp_vfs_spiffs_conf_t conf = {
		.base_path = "/spiffs",
		.partition_label = _PARTITION_NAME,
		.max_files = 3,
		.format_if_mount_failed = false
	};

	ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));

	size_t total = 0, used = 0;
    ESP_ERROR_CHECK(esp_spiffs_info(_PARTITION_NAME, &total, &used));

	FB_DEBUG_LOG_I_TAG("SPIFS total bytes %u, used %u", total, used);

}

void fs::deinit()
{
	ESP_ERROR_CHECK(esp_vfs_spiffs_unregister(_PARTITION_NAME));
}

#include "fb_file_system.hpp"

#include "esp_spiffs.h"

#include "fb_debug.hpp"
#include "fb_settings.hpp"



#define _PARTITION_NAME "storage"



using namespace fb;
using namespace fs;



static const char* TAG = "fb_file_system";



void fs::init()
{
	FB_DEBUG_ENTER_I_TAG();

	const auto partitionName = getSpiffName();

	esp_vfs_spiffs_conf_t conf = {
		.base_path = "/spiffs",
		.partition_label = partitionName.data(),
		.max_files = 3,
		.format_if_mount_failed = true
	};

	ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));

	size_t total = 0, used = 0;
    ESP_ERROR_CHECK(esp_spiffs_info(partitionName.data(), &total, &used));

	FB_DEBUG_LOG_I_TAG("SPIFS total bytes %u, used %u", total, used);

}

void fs::deinit()
{
	ESP_ERROR_CHECK(esp_vfs_spiffs_unregister(getSpiffName().data()));
}

std::string fs::getSpiffName()
{
	std::string result = _PARTITION_NAME;
	if(settings::getSpiffId() != 0){
		result += std::to_string(settings::getSpiffId());
	}

	return result;
}
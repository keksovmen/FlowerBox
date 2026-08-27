#include "fb_update.hpp"

#include "fb_debug.hpp"
#include "fb_file_system.hpp"
#include "fb_globals.hpp"
#include "fb_settings.hpp"

#include "esp_ota_ops.h"
#include "esp_partition.h"

#ifdef _ESP8266
	#define OTA_WITH_SEQUENTIAL_WRITES OTA_SIZE_UNKNOWN
#endif



using namespace fb;
using namespace update;



static const char* TAG = "fb_update";

static esp_ota_handle_t _otaHndl = 0;
static const esp_partition_t* _partition = NULL;
static size_t _dst = 0;
static bool _spiffUpdateStatusFlag = false;



bool update::operator==(int val, UpdateEventId id)
{
	return val == static_cast<int>(id);
}



bool update::begin()
{
	FB_DEBUG_ENTER_I_TAG();

	assert(!_otaHndl);
	assert(!_partition);

	_partition = esp_ota_get_next_update_partition(NULL);
	if(_partition == NULL){
		FB_DEBUG_LOG_E_TAG("Update partition is not found!")
		return false;
	}

	esp_err_t err = esp_ota_begin(_partition, OTA_WITH_SEQUENTIAL_WRITES, &_otaHndl);
	if(err != ESP_OK){
		FB_DEBUG_LOG_E_TAG("Failed to start ota! %d", err);
		return false;
	}

	//drop event
	global::getEventManager()->pushEvent({event::EventGroup::UPDATE, static_cast<int>(UpdateEventId::START), NULL});

	return true;
}

bool update::writeSequential(const char* data, int size)
{
	const esp_err_t err = esp_ota_write(_otaHndl, data, size);
	if(err != ESP_OK){
		FB_DEBUG_LOG_E_TAG("Failed to write ota, must abort! %d", err);
		global::getEventManager()->pushEvent({event::EventGroup::UPDATE, static_cast<int>(UpdateEventId::FAILURE), NULL});

		#ifndef _ESP8266
			esp_ota_abort(_otaHndl);
		#endif

		return false;
	}

	return true;
}

bool update::end()
{
	FB_DEBUG_ENTER_I_TAG();

	esp_err_t err = esp_ota_end(_otaHndl);
	_otaHndl = 0;
	if(err != ESP_OK){
		FB_DEBUG_LOG_E_TAG("Failed to end ota! %d", err);
		_partition = NULL;

		global::getEventManager()->pushEvent({event::EventGroup::UPDATE, static_cast<int>(UpdateEventId::FAILURE), NULL});

		return false;
	}

	err = esp_ota_set_boot_partition(_partition);
	_partition = NULL;
	if(err != ESP_OK){
		FB_DEBUG_LOG_E_TAG("Failed to change start up partition! %d", err);

		global::getEventManager()->pushEvent({event::EventGroup::UPDATE, static_cast<int>(UpdateEventId::FAILURE), NULL});

		return false;
	}

	global::getEventManager()->pushEvent({event::EventGroup::UPDATE, static_cast<int>(UpdateEventId::END), NULL});

	return true;
}



bool update::beginSpiff()
{
	assert(!_partition);

    // Find the partition by subtype
    auto iter = esp_partition_find(
        ESP_PARTITION_TYPE_DATA, 
        ESP_PARTITION_SUBTYPE_DATA_SPIFFS, 
        NULL
    );

	if(!settings::getSpiffId()){
		iter = esp_partition_next(iter);
	}

	if(iter == NULL){
		FB_DEBUG_LOG_E_TAG("SPIFFS partition is not found!");
		return false;
	}

	_partition = esp_partition_get(iter);
	FB_DEBUG_LOG_I_TAG("Partion name to be written: %s", _partition->label);

    if (!_partition){
		FB_DEBUG_LOG_E_TAG("SPIFFS partition is not found!");
		return false;
	}
    // 3. Erase the entire partition (must be done before writing)
    if(esp_partition_erase_range(_partition, 0, _partition->size) != ESP_OK){
		FB_DEBUG_LOG_E_TAG("SPIFFS failed to be erased!");
		return false;
	}

	_dst = 0;
	_spiffUpdateStatusFlag = true;

	return true;
}

bool update::writeSequentialSpiff(const char* data, int size)
{
	if(!esp_partition_write(_partition, _dst, data, size) == ESP_OK){
		FB_DEBUG_LOG_E_TAG("SPIFFS failed to be written!");
		//we failed need to mark it as failure
		_spiffUpdateStatusFlag = false;
		return false;
	}

	_dst += size;
	return true;
}

bool update::endSpiff()
{
	_partition = NULL;

	fs::deinit();

	if(_spiffUpdateStatusFlag){
		//change spiff partition id
		settings::setSpiffId(settings::getSpiffId() ? 0 : 1);

		_spiffUpdateStatusFlag = false;
	}

	fs::init();

	return true;
}
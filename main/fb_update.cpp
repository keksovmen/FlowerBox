#include "fb_update.hpp"

#include "fb_debug.hpp"
#include "fb_globals.hpp"


#include "esp_partition.h"
#include "esp_ota_ops.h"



using namespace fb;
using namespace update;



static const char* TAG = "fb_update";

static esp_ota_handle_t _otaHndl = 0;
const esp_partition_t* _partition = NULL;



bool update::operator==(int val, UpdateEventId id)
{
	return val == static_cast<int>(id);
}



bool update::begin()
{
	FB_DEBUG_TAG_ENTER();
	assert(!_otaHndl);
	assert(!_partition);

	_partition = esp_ota_get_next_update_partition(NULL);
	if(_partition == NULL){
		FB_DEBUG_TAG_LOG_E("Update partition is not found!")
		return false;
	}

	esp_err_t err = esp_ota_begin(_partition, OTA_WITH_SEQUENTIAL_WRITES, &_otaHndl);
	if(err != ESP_OK){
		FB_DEBUG_TAG_LOG_E("Failed to start ota! %d", err);
		return false;
	}

	//drop event
	global::getEventManager()->pushEvent({event::EventGroup::UPDATE, static_cast<int>(UpdateEventId::START), NULL});

	FB_DEBUG_TAG_EXIT();

	return true;
}

bool update::writeSequential(const char* data, int size)
{
	const esp_err_t err = esp_ota_write(_otaHndl, data, size);
	if(err != ESP_OK){
		FB_DEBUG_TAG_LOG_E("Failed to write ota, must abort! %d", err);
		global::getEventManager()->pushEvent({event::EventGroup::UPDATE, static_cast<int>(UpdateEventId::FAILURE), NULL});

		esp_ota_abort(_otaHndl);

		return false;
	}

	return true;
}

bool update::end()
{
	FB_DEBUG_TAG_ENTER();

	esp_err_t err = esp_ota_end(_otaHndl);
	_otaHndl = 0;
	if(err != ESP_OK){
		FB_DEBUG_TAG_LOG_E("Failed to end ota! %d", err);
		_partition = NULL;

		global::getEventManager()->pushEvent({event::EventGroup::UPDATE, static_cast<int>(UpdateEventId::FAILURE), NULL});

		return false;
	}

	err = esp_ota_set_boot_partition(_partition);
	_partition = NULL;
	if(err != ESP_OK){
		FB_DEBUG_TAG_LOG_E("Failed to change start up partition! %d", err);

		global::getEventManager()->pushEvent({event::EventGroup::UPDATE, static_cast<int>(UpdateEventId::FAILURE), NULL});

		return false;
	}

	global::getEventManager()->pushEvent({event::EventGroup::UPDATE, static_cast<int>(UpdateEventId::END), NULL});

	FB_DEBUG_TAG_EXIT();

	return true;
}

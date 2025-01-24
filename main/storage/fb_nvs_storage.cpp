#include "fb_nvs_storage.hpp"

#include "nvs_handle.hpp"

#include "nvs_flash.h"



using namespace fb;
using namespace storage;



NvsStorage::~NvsStorage()
{
	const esp_err_t result = nvs_flash_deinit();
	FB_DEBUG_LOG_I_OBJ("NVS deinit result: %d", static_cast<int>(result));
}

const char* NvsStorage::getName() const
{
	return "NvsStorage";
}

bool NvsStorage::init()
{
	const esp_err_t result = nvs_flash_init();
	FB_DEBUG_LOG_I_OBJ("NVS init result: %d", static_cast<int>(result));

	return result == ESP_OK;
}

bool NvsStorage::hasKey(std::string_view partition, std::string_view key) const
{
	esp_err_t err = ESP_OK;
	auto hndl = nvs::open_nvs_handle(partition.cbegin(), NVS_READONLY, &err);
	if(!hndl){
		return false;
	}

	nvs_type_t _ = NVS_TYPE_ANY;

	err = hndl->find_key(key.cbegin(), _);
	FB_DEBUG_LOG_I_OBJ("Has key: %s = %d", key.cbegin(), static_cast<int>(err));

	return err == ESP_OK;
}

bool NvsStorage::writeValue(std::string_view partition, std::string_view key, std::string_view value)
{
	esp_err_t err = ESP_OK;
	auto hndl = nvs::open_nvs_handle(partition.cbegin(), NVS_READWRITE, &err);
	if(!hndl){
		return false;
	}

	err = hndl->set_string(key.cbegin(), value.cbegin());
	hndl->commit();
	FB_DEBUG_LOG_I_OBJ("Write str: %s -> %s, = %d", key.cbegin(), value.cbegin(), static_cast<int>(err));

	return err == ESP_OK;
}

bool NvsStorage::writeValue(std::string_view partition, std::string_view key, int64_t value)
{
	esp_err_t err = ESP_OK;
	auto hndl = nvs::open_nvs_handle(partition.cbegin(), NVS_READWRITE, &err);
	if(!hndl){
		return false;
	}

	err = hndl->set_item<int>(key.cbegin(), value);
	hndl->commit();
	FB_DEBUG_LOG_I_OBJ("Write int: %s -> %lld, = %d", key.cbegin(), value, static_cast<int>(err));

	return err == ESP_OK;
}

bool NvsStorage::readValue(std::string_view partition, std::string_view key, std::string& out) const
{
	esp_err_t err = ESP_OK;
	auto hndl = nvs::open_nvs_handle(partition.cbegin(), NVS_READWRITE, &err);
	if(!hndl){
		return false;
	}

	char tmp[StorageIface::MAX_STRING_SIZE] = {0};
	err = hndl->get_string(key.cbegin(), tmp, sizeof(tmp));
	out = tmp;
	
	FB_DEBUG_LOG_I_OBJ("Read str: %s -> %s, = %d", key.cbegin(), out.c_str(), static_cast<int>(err));

	return err == ESP_OK;
}

bool NvsStorage::readValue(std::string_view partition, std::string_view key, int64_t& out) const
{
	esp_err_t err = ESP_OK;
	auto hndl = nvs::open_nvs_handle(partition.cbegin(), NVS_READWRITE, &err);
	if(!hndl){
		return false;
	}

	err = hndl->get_item<int64_t>(key.cbegin(), out);

	FB_DEBUG_LOG_I_OBJ("Read int: %s -> %lld, = %d", key.cbegin(), out, static_cast<int>(err));

	return err == ESP_OK;
}
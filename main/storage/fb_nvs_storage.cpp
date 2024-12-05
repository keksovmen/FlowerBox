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

bool NvsStorage::hasKey(const std::string& partition, const std::string& key) const
{
	esp_err_t err = ESP_OK;
	auto hndl = nvs::open_nvs_handle(partition.c_str(), NVS_READONLY, &err);
	if(!hndl){
		return false;
	}

	nvs_type_t _ = NVS_TYPE_ANY;

	err = hndl->find_key(key.c_str(), _);
	FB_DEBUG_LOG_I_OBJ("Has key: %s = %d", key.c_str(), static_cast<int>(err));

	return err == ESP_OK;
}

bool NvsStorage::writeValue(const std::string& partition, const std::string& key, const std::string& value)
{
	esp_err_t err = ESP_OK;
	auto hndl = nvs::open_nvs_handle(partition.c_str(), NVS_READWRITE, &err);
	if(!hndl){
		return false;
	}

	err = hndl->set_string(key.c_str(), value.c_str());
	hndl->commit();
	FB_DEBUG_LOG_I_OBJ("Write str: %s -> %s, = %d", key.c_str(), value.c_str(), static_cast<int>(err));

	return err == ESP_OK;
}

bool NvsStorage::writeValue(const std::string& partition, const std::string& key, int value)
{
	esp_err_t err = ESP_OK;
	auto hndl = nvs::open_nvs_handle(partition.c_str(), NVS_READWRITE, &err);
	if(!hndl){
		return false;
	}

	err = hndl->set_item<int>(key.c_str(), value);
	hndl->commit();
	FB_DEBUG_LOG_I_OBJ("Write int: %s -> %d, = %d", key.c_str(), value, static_cast<int>(err));

	return err == ESP_OK;
}

bool NvsStorage::readValue(const std::string& partition, const std::string& key, std::string& out) const
{
	esp_err_t err = ESP_OK;
	auto hndl = nvs::open_nvs_handle(partition.c_str(), NVS_READWRITE, &err);
	if(!hndl){
		return false;
	}

	char tmp[StorageIface::MAX_STRING_SIZE] = {0};
	err = hndl->get_string(key.c_str(), tmp, sizeof(tmp));
	out = tmp;
	
	FB_DEBUG_LOG_I_OBJ("Read str: %s -> %s, = %d", key.c_str(), out.c_str(), static_cast<int>(err));

	return err == ESP_OK;
}

bool NvsStorage::readValue(const std::string& partition, const std::string& key, int& out) const
{
	esp_err_t err = ESP_OK;
	auto hndl = nvs::open_nvs_handle(partition.c_str(), NVS_READWRITE, &err);
	if(!hndl){
		return false;
	}

	err = hndl->get_item<int>(key.c_str(), out);

	FB_DEBUG_LOG_I_OBJ("Read int: %s -> %d, = %d", key.c_str(), out, static_cast<int>(err));

	return err == ESP_OK;
}
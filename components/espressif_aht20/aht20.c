/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "aht20.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"

#include "aht20_reg.h"



#define _TIMEOUT_MS 1000
#define _CLOCK_HZ 10000



typedef struct {
	i2c_master_bus_handle_t bus;
	i2c_master_dev_handle_t dev;
} aht20_dev_t;



const static char *TAG = "AHT20";



static esp_err_t _aht20_write_reg(aht20_dev_handle_t dev, uint8_t *data, uint8_t len)
{
	aht20_dev_t *sens = (aht20_dev_t *) dev;
	esp_err_t  ret = ESP_OK;

	if(len > 0){
		ret = i2c_master_transmit(sens->dev, data, len, _TIMEOUT_MS);
		ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
	}

	return ret;
}

static esp_err_t _aht20_read_reg(aht20_dev_handle_t dev, uint8_t *data, size_t len)
{
	aht20_dev_t *sens = (aht20_dev_t *) dev;

	esp_err_t ret = i2c_master_receive(sens->dev, data, len, _TIMEOUT_MS);
	ESP_ERROR_CHECK_WITHOUT_ABORT(ret);

	return ret;
}

static uint8_t _aht20_calc_crc(uint8_t *data, uint8_t len)
{
	uint8_t i;
	uint8_t byte;
	uint8_t crc = 0xFF;

	for (byte = 0; byte < len; byte++) {
		crc ^= data[byte];
		for (i = 8; i > 0; --i) {
			if ((crc & 0x80) != 0) {
				crc = (crc << 1) ^ 0x31;
			} else {
				crc = crc << 1;
			}
		}
	}

	return crc;
}

esp_err_t aht20_read_temperature_humidity(aht20_dev_handle_t handle,
										  float *temperature,
										  float *humidity)
{
	uint8_t status;
	uint8_t buf[7];
	uint32_t raw_data;

	ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid device handle pointer");

	buf[0] = AHT20_START_MEASURMENT_CMD;
	buf[1] = 0x33;
	buf[2] = 0x00;
	ESP_RETURN_ON_ERROR(_aht20_write_reg(handle, buf, 3), TAG, "I2C write error");

	vTaskDelay(pdMS_TO_TICKS(100));

	ESP_RETURN_ON_ERROR(_aht20_read_reg(handle, &status, 1), TAG, "I2C read error");

	if ((status & BIT(AT581X_STATUS_CALIBRATION_ENABLE)) &&
			(status & BIT(AT581X_STATUS_CRC_FLAG)) &&
			((status & BIT(AT581X_STATUS_BUSY_INDICATION)) == 0)) {
		ESP_RETURN_ON_ERROR(_aht20_read_reg(handle, buf, 7), TAG, "I2C read error");
		ESP_RETURN_ON_ERROR((_aht20_calc_crc(buf, 6) != buf[6]), TAG, "crc is error");

		raw_data = buf[1];
		raw_data = raw_data << 8;
		raw_data += buf[2];
		raw_data = raw_data << 8;
		raw_data += buf[3];
		raw_data = raw_data >> 4;
		*humidity = (float) raw_data * 100 / 1048576;

		raw_data = buf[3] & 0x0F;
		raw_data = raw_data << 8;
		raw_data += buf[4];
		raw_data = raw_data << 8;
		raw_data += buf[5];
		*temperature = (float) raw_data * 200 / 1048576 - 50;
		return ESP_OK;
	} else {
		ESP_LOGI(TAG, "data is not ready");
		return ESP_ERR_NOT_FINISHED;
	}
}

esp_err_t aht20_new_sensor(const aht20_i2c_config_t *i2c_conf, aht20_dev_handle_t *handle_out)
{
	ESP_LOGI(TAG, "%-15s", CHIP_NAME);
	ESP_LOGI(TAG, "%-15s: %1.1f - %1.1fV", "SUPPLY_VOLTAGE", SUPPLY_VOLTAGE_MIN, SUPPLY_VOLTAGE_MAX);
	ESP_LOGI(TAG, "%-15s: %.2f - %.2f℃", "TEMPERATURE", TEMPERATURE_MIN, TEMPERATURE_MAX);

	ESP_RETURN_ON_FALSE(i2c_conf, ESP_ERR_INVALID_ARG, TAG, "invalid device config pointer");
	ESP_RETURN_ON_FALSE(handle_out, ESP_ERR_INVALID_ARG, TAG, "invalid device handle pointer");

	i2c_device_config_t dev_cfg = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = AHT20_ADDRRES,
		.scl_speed_hz = _CLOCK_HZ,
		.scl_wait_us = 0,
		.flags = {.disable_ack_check = 1},
	};
	i2c_master_dev_handle_t dev = NULL;
	ESP_RETURN_ON_FALSE(i2c_master_bus_add_device(i2c_conf->bus, &dev_cfg, &dev) == ESP_OK, ESP_ERR_INVALID_ARG, TAG, "Invalid bus device");

	aht20_dev_t *handle = calloc(1, sizeof(aht20_dev_t));
	ESP_RETURN_ON_FALSE(handle, ESP_ERR_NO_MEM, TAG, "memory allocation for device handler failed");

	handle->bus = i2c_conf->bus;
	handle->dev = dev;

	*handle_out = handle;

	return ESP_OK;
}

esp_err_t aht20_del_sensor(aht20_dev_handle_t handle)
{
	ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid device handle pointer");

	i2c_master_bus_rm_device(((aht20_dev_t*) handle)->dev);
	free(handle);

	return ESP_OK;
}

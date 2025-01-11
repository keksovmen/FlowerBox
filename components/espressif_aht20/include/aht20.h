/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once



#ifdef __cplusplus
extern "C" {
#endif

#include "esp_types.h"
#include "esp_err.h"

#include "driver/i2c.h"
#include "driver/i2c_master.h"



#define AHT20_ADDRRES (0x38)



/**
 * @brief Type of AHT20 device handle
 */
typedef void* aht20_dev_handle_t;

/**
 * @brief AHT20 I2C config struct
 *
 */
typedef struct {
    i2c_master_bus_handle_t  bus;           /*!< I2C port used to connected AHT20 device */
} aht20_i2c_config_t;

/**
 * @brief Create new AHT20 device handle.
 *
 * @param[in]  i2c_conf Config for I2C used by AHT20
 * @param[out] handle_out New AHT20 device handle
 * @return
 *          - ESP_OK                  Device handle creation success.
 *          - ESP_ERR_INVALID_ARG     Invalid device handle or argument.
 *          - ESP_ERR_NO_MEM          Memory allocation failed.
 *
 */
esp_err_t aht20_new_sensor(const aht20_i2c_config_t *i2c_conf, aht20_dev_handle_t *handle_out);

/**
 * @brief Delete AHT20 device handle.
 *
 * @param[in] handle AHT20 device handle
 * @return
 *          - ESP_OK                  Device handle deletion success.
 *          - ESP_ERR_INVALID_ARG     Invalid device handle or argument.
 *
 */
esp_err_t aht20_del_sensor(aht20_dev_handle_t handle);

/**
 * @brief read the temperature and humidity data
 *
 * @param[in]  *handle points to an aht20 handle structure
 * @param[out] *temperature points to a converted temperature buffer
 * @param[out] *humidity points to a converted humidity buffer
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Fail
 */
esp_err_t aht20_read_temperature_humidity(aht20_dev_handle_t handle,
                                          float *temperature,
                                          float *humidity);
#ifdef __cplusplus
}
#endif

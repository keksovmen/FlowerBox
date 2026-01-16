#include "fb_i2c.hpp"

#include "fb_debug.hpp"



using namespace fb;
using namespace interfaces;



static const char* TAG = "I2c";



void I2c::init(i2c_port_num_t port, int sdaPin, int sclPin)
{
	i2c_master_bus_config_t cfg = {
		.i2c_port = 0,
		.sda_io_num = static_cast<gpio_num_t>(sdaPin),
		.scl_io_num = static_cast<gpio_num_t>(sclPin),
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 7,
		.intr_priority = 0,
		.trans_queue_depth = 0,
		.flags = {
			.enable_internal_pullup = true,
			.allow_pd = false,
		},
	};
	ESP_ERROR_CHECK(i2c_new_master_bus(&cfg, &_i2cBus));
}

bool I2c::read(int device, std::span<uint8_t> out, int timeoutMs)
{
	esp_err_t err = i2c_master_receive(_devices[device], out.data(), out.size(), timeoutMs);
	if(err != ESP_OK){
		FB_DEBUG_LOG_E_TAG("Read err: %d = %s", err, esp_err_to_name(err));
	}

	return err == ESP_OK;
}

bool I2c::write(int device, std::span<uint8_t> in, int timeoutMs)
{
	esp_err_t err = i2c_master_transmit(_devices[device], in.data(), in.size(), timeoutMs);
	if(err != ESP_OK){
		FB_DEBUG_LOG_E_TAG("Write err: %d = %s", err, esp_err_to_name(err));
	}

	return err == ESP_OK;
}

int I2c::addDevice(uint32_t speedHz, uint16_t address)
{
	i2c_device_config_t devCfg = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = address,
		.scl_speed_hz = speedHz,
		.scl_wait_us = 200000,
		.flags = {.disable_ack_check = 0},
	};
	i2c_master_dev_handle_t hndl;
	ESP_ERROR_CHECK(i2c_master_bus_add_device(_i2cBus, &devCfg, &hndl));

	_devices.push_back(hndl);

	return _devices.size() - 1;
}

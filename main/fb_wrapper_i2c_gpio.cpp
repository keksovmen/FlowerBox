#include "fb_wrapper_i2c_gpio.hpp"



#include "driver/i2c.h"



using namespace fb;
using namespace wrappers;



WrapperI2cGpio::WrapperI2cGpio(int gpioScl, int gpioSda)
	: _gpioScl(gpioScl), _gpioSda(gpioSda)
{

}

void WrapperI2cGpio::init()
{
	i2c_config_t conf;
	#ifdef _ESP8266
		conf.clk_stretch_tick = 300; // 300 ticks, Clock stretch is about 210us, you can make changes according to the actual situation.
	#else
		conf.master.clk_speed = 100000;
	#endif
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = static_cast<gpio_num_t>(_gpioSda);
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = static_cast<gpio_num_t>(_gpioScl);
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;

	#ifdef _ESP8266
		ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, conf.mode));
	#else
		ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0));
	#endif
	
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
}

void WrapperI2cGpio::setValue(bool value)
{
	setValue(0xFFFF);
}

void WrapperI2cGpio::setValue(int value)
{
	uint8_t data[2] = {(uint8_t)(value & 0xFF), (uint8_t)((value >> 8) & 0xFF)};

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (0x20 << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write(cmd, data, sizeof(data), true);
	i2c_master_stop(cmd);
	
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
}

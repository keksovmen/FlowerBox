#include "fb_rgb_switch.hpp"



using namespace fb;
using namespace switches;



RgbSwitch::RgbSwitch(ledc_timer_t timer, ledc_channel_t startChannel, int gpioR, int gpioG, int gpioB)
	: SwitchIface(&_condition, &_action),
	_timer(timer), _startChannel(startChannel)
{
	ledc_timer_config_t timerCfg = {
		.speed_mode = LEDC_LOW_SPEED_MODE,
		.duty_resolution = LEDC_TIMER_8_BIT,
		.timer_num = _timer,
		.freq_hz = 4000,
		.clk_cfg = LEDC_AUTO_CLK,
		.deconfigure = false,
	};
	ESP_ERROR_CHECK(ledc_timer_config(&timerCfg));

	ledc_channel_config_t channelCfg = {
		.gpio_num = gpioR,
		.speed_mode = LEDC_LOW_SPEED_MODE,
		.channel = startChannel,
		.intr_type = LEDC_INTR_DISABLE,
		.timer_sel = timer,
		.duty = 0,
		.hpoint = 0,
		.flags = 0,
	};
	ESP_ERROR_CHECK(ledc_channel_config(&channelCfg));

	channelCfg.gpio_num = gpioG;
	channelCfg.channel = static_cast<ledc_channel_t>(_startChannel + 1);
	ESP_ERROR_CHECK(ledc_channel_config(&channelCfg));

	channelCfg.gpio_num = gpioB;
	channelCfg.channel = static_cast<ledc_channel_t>(_startChannel + 2);
	ESP_ERROR_CHECK(ledc_channel_config(&channelCfg));
}

const char* RgbSwitch::getName() const
{
	return "RgbSwitch";
}

void RgbSwitch::setColor(int color)
{
	_color = color;

	if(isOn()){
		_applyColor(getColor());
	}
}

int RgbSwitch::getColor() const
{
	return _color;
}

void RgbSwitch::_applyColor(int color)
{
	const int red = (color >> 16) & 0xFF;
	const int green = (color >> 8) & 0xFF;
	const int blue = color & 0xFF;

	FB_DEBUG_LOG_I_OBJ("Apply color: R = %d, G = %d, B = %d", red, green, blue);

	ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, _startChannel, red));
	ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, static_cast<ledc_channel_t>(_startChannel + 1), green));
	ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, static_cast<ledc_channel_t>(_startChannel + 2), blue));

	ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, _startChannel));
	ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, static_cast<ledc_channel_t>(_startChannel + 1)));
	ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, static_cast<ledc_channel_t>(_startChannel + 2)));
}

bool RgbSwitch::_condition(SwitchIface* me)
{
	return false;
}

void RgbSwitch::_action(SwitchIface* me, bool value)
{
	RgbSwitch* ptr = reinterpret_cast<RgbSwitch*>(me);

	if(value){
		ptr->_applyColor(ptr->getColor());
	}else{
		ptr->_applyColor(0);
	}
}



RgbSwitchDmx::RgbSwitchDmx(dmx_port_t port, int gpioRX, int gpioTX, int gpioRTS)
	: SwitchIface(&_condition, &_action),
	_dmx(port), _gpioRX(gpioRX), _gpioTX(gpioTX), _gpioRTS(gpioRTS)
{
	
}

RgbSwitchDmx::~RgbSwitchDmx()
{
	dmx_driver_delete(_dmx);
}

const char* RgbSwitchDmx::getName() const
{
	return "RgbSwitchDmx";
}

void RgbSwitchDmx::init()
{
	dmx_config_t config = DMX_CONFIG_DEFAULT;
	config.queue_size_max = 0;
	
	// ...install the DMX driver...
	dmx_driver_install(_dmx, &config, NULL, 0);
	dmx_set_pin(_dmx, _gpioTX, _gpioRX, _gpioRTS);
	dmx_set_baud_rate(_dmx, 250000);
	dmx_set_break_len(_dmx, 88);
	dmx_set_mab_len(_dmx, 8);
}

void RgbSwitchDmx::setColor(int color)
{
	_color = color;

	if(isOn()){
		_applyColor(getColor());
	}
}

int RgbSwitchDmx::getColor() const
{
	return _color;
}

void RgbSwitchDmx::_applyColor(int color)
{
	const uint8_t white = (color >> 24) & 0xFF;
	const uint8_t red = (color >> 16) & 0xFF;
	const uint8_t green = (color >> 8) & 0xFF;
	const uint8_t blue = color & 0xFF;

	FB_DEBUG_LOG_I_OBJ("Apply color: R = %u, G = %u, B = %u, W = %u", red, green, blue, white);

	uint8_t packet[4] = {red, green, blue, white};

	dmx_write_slot(_dmx, 0, 0);
	dmx_write_offset(_dmx, _address, packet, sizeof(packet));
}

bool RgbSwitchDmx::_condition(SwitchIface* me)
{
	return false;
}

void RgbSwitchDmx::_action(SwitchIface* me, bool value)
{
	RgbSwitchDmx* ptr = reinterpret_cast<RgbSwitchDmx*>(me);

	if(value){
		ptr->_applyColor(ptr->getColor());
	}else{
		ptr->_applyColor(0);
	}
}

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
	const int red = color & 0xFF;
	const int green = (color >> 8) & 0xFF;
	const int blue = (color >> 16) & 0xFF;

	FB_DEBUG_LOG("Apply color: R = %d, G = %d, B = %d", red, green, blue);

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

#include "fb_dmx_hal.hpp"

#include <algorithm>



using namespace fb;
using namespace periph;



const char* DmxHal::getName() const
{
	return "DmxHal";
}

void DmxHal::init(dmx_port_t port, int gpioRX, int gpioTX, int gpioRTS)
{
	FB_DEBUG_LOG_I_OBJ("Init dmx: port = %d, RX = %d, TX = %d, RTS = %d",
		port, gpioRX, gpioTX, gpioRTS);
	
	dmx_config_t config = DMX_CONFIG_DEFAULT;
	config.queue_size_max = 0;
	
	dmx_driver_install(port, &config, NULL, 0);
	dmx_set_pin(port, gpioTX, gpioRX, gpioRTS);
	dmx_set_baud_rate(port, 250000);
	dmx_set_break_len(port, 88);
	dmx_set_mab_len(port, 8);

	_dmx = port;
	memset(_buffer, 0, sizeof(_buffer));

	send();
}

void DmxHal::send()
{
	dmx_write_slot(_dmx, 0, 0);
	dmx_write_offset(_dmx, 1, _buffer, sizeof(_buffer));
	dmx_send(_dmx);
}

void DmxHal::write(uint16_t index, uint8_t data)
{
	if(index >= 512){
		FB_DEBUG_LOG_E_OBJ("Illegal index: %u", index);
		return;
	}

	_buffer[index] = data;
}

void DmxHal::write(uint16_t index, std::span<uint8_t> data)
{
	if(index >= 512){
		FB_DEBUG_LOG_E_OBJ("Illegal index: %u", index);
		return;
	}

	const uint16_t size = std::max((int) data.size(), 512 - index);

	memcpy(&_buffer[index], data.data(), size);
}
#pragma once



#include <span>

#include "fb_debug.hpp"

#include "esp_dmx.h"



namespace fb
{
	namespace periph
	{
		class DmxHal : public debug::Named
		{
			public:
				virtual const char* getName() const override;

				void init(dmx_port_t port, int gpioRX, int gpioTX, int gpioRTS);

				void send();

				void write(uint16_t index, uint8_t data);
				void write(uint16_t index, std::span<uint8_t> data);

			private:
				dmx_port_t _dmx;

				uint8_t _buffer[512];
		};
	}
}
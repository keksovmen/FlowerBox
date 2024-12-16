#pragma once



#include <span>

#include "driver/uart.h"



namespace fb
{
	namespace interfaces
	{
		class Uart
		{
			public:
				Uart(uart_port_t port, int rx, int tx, int baudrate);
				~Uart();

				bool read(std::span<uint8_t> out, int timeoutMs);
				bool write(std::span<uint8_t> in);
			
			private:
				const uart_port_t _port;
		};
	}
}
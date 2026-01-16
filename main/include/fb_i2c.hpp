#pragma once



#include <span>
#include <vector>

#include "driver/i2c_master.h"



namespace fb
{
	namespace interfaces
	{
		class I2c
		{
			public:
				static const int FAILED = -1;



				// ~I2c();

				void init(i2c_port_num_t port, int sdaPin, int sclPin);
				bool read(int device, std::span<uint8_t> out, int timeoutMs);
				bool write(int device, std::span<uint8_t> in, int timeoutMs);
				int addDevice(uint32_t speedHz, uint16_t address);
			
			private:
				i2c_master_bus_handle_t _i2cBus = nullptr;
				std::vector<i2c_master_dev_handle_t> _devices;
		};
	}
}
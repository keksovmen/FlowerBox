#pragma once



#include "fb_wrappers.hpp"

#include "driver/spi_master.h"



namespace fb
{
	namespace wrappers
	{
		class WrapperDb135 : public WrapperIface
		{
			public:
				WrapperDb135(int scl, int sda, int cs);

				virtual void init() override;

				virtual void setValue(bool value) override;
				virtual void setValue(int value) override;

				void setPin(int pin, bool isOn);
				uint16_t getValue() const;
			
			private:
				const int _gpioScl;
				const int _gpioSda;
				const int _gpioCs;

				spi_device_handle_t _dev = nullptr;
				uint16_t _state = 0;



				void _sendState();
		};
	}
}
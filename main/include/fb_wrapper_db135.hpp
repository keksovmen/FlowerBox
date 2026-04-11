#pragma once



#include <array>
#include <span>

#include "fb_wrappers.hpp"

#include "driver/spi_master.h"



namespace fb
{
	namespace wrappers
	{
		template<int ChipsCount>
		class WrapperDb135 : public WrapperIface
		{
			public:
				WrapperDb135(int scl, int sda, int cs);

				virtual void init() override;

				virtual void IRAM_ATTR setValue(bool value) override;
				virtual void IRAM_ATTR setValue(int value) override;
				void IRAM_ATTR setValue(std::span<uint16_t, ChipsCount> value);

				void setPin(int pin, bool isOn);
				uint16_t getValue() const;
			
			private:
				const int _gpioScl;
				const int _gpioSda;
				const int _gpioCs;

				spi_device_handle_t _dev = nullptr;
				std::array<uint16_t, ChipsCount> _states = {0};



				void IRAM_ATTR _sendState();
		};

		template class WrapperDb135<1>;
		template class WrapperDb135<2>;
	}
}
#pragma once



#include <vector>

#include "fb_debug.hpp"
#include "fb_ir_commands.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"



namespace fb
{
	namespace periph
	{
		class IrTransmitter : public debug::Named
		{
			public:
				virtual const char* getName() const override;
				
				void addGpio(int gpio);
				
				void sendHeal(int gpio);
				void sendKill(int gpio);
				void sendAttack(int gpio, Ir::AttackCmd attack);

			private:
				//for enter critical section
				static portMUX_TYPE _lock;

				//just to be sure we use what we registered
				std::vector<int> _gpios;



				static void IRAM_ATTR _delayUs(int64_t us);

				static void IRAM_ATTR _sendCarrier(int gpio, int times);
				static void IRAM_ATTR _sendStart(int gpio);
				static void IRAM_ATTR _sendZero(int gpio);
				static void IRAM_ATTR _sendOne(int gpio);
				static void IRAM_ATTR _sendByte(int gpio, uint8_t data);
				static void IRAM_ATTR _sendBytes(int gpio, const uint8_t* data, int length);

				bool _hasGpio(int gpio);
		};
	}
}

#pragma once

#include <stdint.h>



#define FB_IR_COMMANDS_HEAL 0x8305E8
#define FB_IR_COMMANDS_KILL 0x8300E8
#define FB_IR_COMMANDS_GLOBAL_LENGTH_BITS 24
#define FB_IR_COMMANDS_ATTACK_LENGTH_BITS 14



namespace fb
{
	namespace periph
	{
		namespace Ir
		{
			enum class Team : int
			{
				RED = 0,
				BLUE = 0b01,
				YELLOW = 0b10,
				GREEN = 0b11,
			};


			
			struct __attribute__ ((packed)) AttackPacket 
			{
				uint16_t damage : 4;
				uint16_t team : 2;
				uint16_t id : 7;
				uint16_t _zero : 3;
			};

			union AttackCmd
			{
				AttackPacket packet;
				uint16_t raw;

				AttackCmd(int v) : raw(v)
				{

				}
			};


			//must you when you send data
			inline uint32_t reverseInt(uint32_t val, int length)
			{
				uint32_t result = 0;
				for(int i = 0; i < length; i++){
					result |= ((val >> i) & 0x01) << (length - 1 - i);
				}

				return result;
			}
		}
	}
}
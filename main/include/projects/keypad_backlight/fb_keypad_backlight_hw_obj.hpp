#pragma once



#include "fb_core.hpp"
#include "fb_wrapper_db135.hpp"



namespace fb
{
	namespace project
	{
		wrappers::WrapperDb135& getHwWrapperDb();
		void setDbState(uint16_t state);
		uint16_t getDbState();
		void setPulseMode(int pin);
	}
}
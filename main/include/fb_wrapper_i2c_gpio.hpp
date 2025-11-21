#pragma once



#include "fb_wrappers.hpp"



namespace fb
{
	namespace wrappers
	{
		class WrapperI2cGpio : public WrapperIface
		{
			public:
				WrapperI2cGpio(int gpioScl, int gpioSda);

				virtual void init() override;

				virtual void setValue(bool value) override;
				virtual void setValue(int value) override;
			
			private:
				const int _gpioScl;
				const int _gpioSda;
		};
	}
}
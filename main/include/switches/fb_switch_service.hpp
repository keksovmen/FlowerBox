#pragma once



#include <vector>

#include "fb_service_iface.hpp"
#include "fb_switch_iface.hpp"



namespace fb
{
	namespace switches
	{
		class SwitchService : public util::ServiceIface
		{
			public:
				virtual const char* getName() const override;

				void addSwitch(SwitchIface* entry);
			
			private:
				std::vector<SwitchIface*> _entries;



				//задача
				virtual void _onPull() override;
		};
	}
}
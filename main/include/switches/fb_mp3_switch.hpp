#pragma once



#include "fb_switch_iface.hpp"



namespace fb
{
	namespace switches
	{
		class Mp3Switch : public SwitchIface
		{
			public:
				Mp3Switch();

				virtual const char* getName() const override;

				bool play(int trackId);
				void stop();
				
				int getTrack() const;

			private:
				static bool _condition(SwitchIface* me);
				static void _action(SwitchIface* me, bool value);

		};
	}
}
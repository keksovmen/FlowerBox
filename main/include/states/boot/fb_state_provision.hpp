#pragma once



#include "fb_state_manager.hpp"

#include "esp_netif_types.h"



namespace fb
{
	namespace state
	{
		enum class ProvisionEventId
		{
			SUCCESS,
			FAILURE,
		};
		constexpr bool operator==(int val, ProvisionEventId e){
			return val == static_cast<int>(e);
		}


		/**
		 * @brief initializes all hardware that is required
		 */

		class StateProvision : public State
		{
			public:
				StateProvision(StateManager& context);

				virtual const char* getName() override;

				virtual void handleEvent(const event::Event& event) override;

				virtual void enter() override;
				virtual void exit() override;
			
			private:
				esp_netif_t* _staNetif = nullptr;
				esp_netif_t* _apNetif = nullptr;



				static void _eventHandler(void* arg, esp_event_base_t eventBase, int32_t eventId, void* eventData);
				static esp_err_t _customProvisionHandler(uint32_t sessionId, const uint8_t* inBuf, ssize_t inLen,
							uint8_t** outBuf, ssize_t* outLen, void* arg);



				void _initWifi();
				void _initProvision();

				void _deinitWifi();
				void _deinitProvision();

		};
	}
}
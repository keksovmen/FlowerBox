#pragma once

#include <string>

#include "esp_err.h"



namespace fb
{
	namespace wifi
	{
		enum class WifiEventId : int
		{
			CONNECTED,
			FAILED_TO_CONNECT,
			DISCONNECTED,
		};
		bool operator==(int val, WifiEventId id);



		void init();

		// void start();
		// void stop();
		esp_err_t connect(const std::string& ssid, const std::string& password);
		esp_err_t shutdown();
	}
}
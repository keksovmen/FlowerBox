#pragma once

#include <string>

#include "esp_err.h"



namespace fb
{
	namespace wifi
	{
		void init();

		void start();
		void stop();
		esp_err_t connect(const std::string& ssid, const std::string& password);
		esp_err_t shutdown();
	}
}
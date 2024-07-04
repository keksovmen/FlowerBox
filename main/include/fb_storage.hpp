#pragma once



/**
 * @brief must provide nvs partitions and keys
 */



#include <string>

#include "fb_settings.hpp"



namespace fb
{
	namespace storage
	{
		bool init();

		//getters, return false if no such value is present
		bool getApSsid(std::string& out);
		bool getApPass(std::string& out);

		bool getStaSsid(std::string& out);
		bool getStaPass(std::string& out);

		bool getWifiMode(settings::WifiMode& out);


		//setters
		void setApSsid(const std::string& val);
		void setApPass(const std::string& val);

		void setStaSsid(const std::string& val);
		void setStaPass(const std::string& val);

		void setWifiMode(settings::WifiMode val);
	}
}
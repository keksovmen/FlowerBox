#pragma once



#include <memory>
#include <string>

#include "fb_storage.hpp"



/**
 * @brief in implementation must get values if there is no saved
 * once, then use default once
 * 
 */


namespace fb
{
	namespace settings
	{
		enum class WifiMode : int
		{
			AP,
			STA,
			MAX
		};



		struct NetworkConfig
		{
			WifiMode mode;
			
			std::string apSsid;
			std::string apPass;

			std::string staSsid;
			std::string staPass;
		};



		void init(std::unique_ptr<storage::StorageIface> storage);

		//getters
		std::string getApSsid();
		std::string getApPass();

		std::string getStaSsid();
		std::string getStaPass();

		WifiMode getWifiMode();

		std::string getSntpServerUrl();

		bool isWifiProvided();


		//setters
		void setApSsid(const std::string& val);
		void setApPass(const std::string& val);

		void setStaSsid(const std::string& val);
		void setStaPass(const std::string& val);

		void setWifiMode(WifiMode val);

		void setSntpPass(const std::string& val);

		void setWifiProvisioned();
	} // namespace settings
	
}
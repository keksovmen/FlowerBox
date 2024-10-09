#pragma once



#include <string>



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
			STA
		};



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
	} // namespace settings
	
}
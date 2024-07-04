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
		const std::string& getApSsid();
		const std::string& getApPass();

		const std::string& getStaSsid();
		const std::string& getStaPass();

		WifiMode getWifiMode();


		//setters
		void setApSsid(const std::string& val);
		void setApPass(const std::string& val);

		void setStaSsid(const std::string& val);
		void setStaPass(const std::string& val);

		void setWifiMode(WifiMode val);
	} // namespace settings
	
}
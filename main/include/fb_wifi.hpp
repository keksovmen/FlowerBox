#pragma once



#include <string>

#include "esp_err.h"



namespace fb
{
	namespace wifi
	{
		enum class WifiEventId : int
		{
			CONNECTED,			//when successfully connected as STA or somebody is connected as AP
			FAILED_TO_CONNECT,
			DISCONNECTED,
			AP_CONNECTED,		//when somebody connects to AP
			AP_DISCONNECTED,	//when somebody disconnects from AP
			STA_CONNECTED,		//when successfully connects to AP
			STA_DISCONNECTED,	//when disconnected from AP
			SCAN_FINISHED,		//when successfully finished
			SCAN_FAILED,		//when scan failed
		};
		bool operator==(int val, WifiEventId id);



		esp_err_t connect(const std::string& ssid, const std::string& password);
		esp_err_t shutdown();



		enum class WifiState : int
		{
			NONE = 0,
			AP,
			STA,
			SCAN
		};



		struct WifiConfig
		{
			std::string apSsid;		//len > 8
			std::string apPass;		//len > 8 or 0 for no pass
			std::string staSsid;	//len > 8
			std::string staPass;	//len > 8 or 0 for no pass
			WifiState state;		//initial state
		};



		/**
		 * @brief must be called only once
		 * Init wifi state
		 * Does not start it
		 * 
		 * @return true 
		 * @return false esp errors
		 */

		bool init();

		/**
		 * @brief if state is:
		 * 		AP - will create access point
		 * 		STA - will connect to AP,
		 * 		SCAN - will start network scan
		 * 
		 * @param cfg 
		 * @return true
		 * @return false failed in any way possible
		 */

		bool start(const WifiConfig& cfg);

		/**
		 * @brief if state is:
		 * 		AP - will disconnect all STA and deinit AP
		 * 		STA - will disconnect and deinit
		 * 		SCAN - will cancel request and clear previous result
		 */

		void stop();

		/**
		 * @brief changes next start state
		 * Has no immediate effect
		 * 
		 * @param state next state
		 */

		void changeState(WifiState state);

		/**
		 * @brief restarts wifi even if current state == old state
		 * 
		 * @return true 
		 * @return false failed in any way possible
		 */

		bool restart();

		/**
		 * @brief changes next AP config
		 * Has no immediate effect
		 * 
		 * @param ssid len > 8
		 * @param pass len > 8 or 0 for no pass
		 * @return true 
		 * @return false arguments have illegal values
		 */

		bool setApCfg(const std::string& ssid, const std::string& pass);

		/**
		 * @brief changes next STA config
		 * Has no immediate effect
		 * 
		 * @param ssid len > 8
		 * @param pass len > 8 or 0 for no pass
		 * @return true 
		 * @return false arguments have illegal values
		 */

		bool setStaCfg(const std::string& ssid, const std::string& pass);
	}
}
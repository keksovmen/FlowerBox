#pragma once



#include <string>



namespace fb
{
	namespace nvs
	{
		/**
		 * @brief must be called once
		 * 
		 * @return true 
		 * @return false failed to init NVS for any reason
		 */

		bool init();

		/**
		 * @brief checks if such a key is exist
		 * 
		 * @param partition 
		 * @param key 
		 * @return true exists
		 * @return false doesn't exist
		 */

		bool hasKey(const std::string& partition, const std::string& key);

		/**
		 * @brief writes value by a key
		 * 
		 * @param partition 
		 * @param key 
		 * @param value 
		 * @return true 
		 * @return false failed in any way to write
		 */

		bool writeValue(const std::string& partition, const std::string& key, const std::string& value);
		bool writeValue(const std::string& partition, const std::string& key, int value);

		/**
		 * @brief reads value by a key
		 * 
		 * @param partition 
		 * @param key 
		 * @param out result 
		 * @return true 
		 * @return false failed in any way to write
		 */

		bool readValue(const std::string& partition, const std::string& key, std::string& out);
		bool readValue(const std::string& partition, const std::string& key, int& out);

	} // namespace nvs
}
#pragma once



/**
 * @brief must provide nvs partitions and keys
 */



#include <string>

#include "fb_debug.hpp"
// #include "fb_settings.hpp"



namespace fb
{
	namespace storage
	{
		class StorageIface : public debug::Named
		{
			public:
				static constexpr int MAX_STRING_SIZE = 64;


				
				virtual ~StorageIface(){}

				/**
				 * @brief must be called once
				 * 
				 * @return true 
				 * @return false failed to init NVS for any reason
				 */

				virtual bool init() = 0;

				/**
				 * @brief checks if such a key is exist
				 * 
				 * @param partition 
				 * @param key 
				 * @return true exists
				 * @return false doesn't exist
				 */

				virtual bool hasKey(const std::string& partition, const std::string& key) const = 0;

				/**
				 * @brief writes value by a key
				 * 
				 * @param partition 
				 * @param key 
				 * @param value 
				 * @return true 
				 * @return false failed in any way to write
				 */

				virtual bool writeValue(const std::string& partition, const std::string& key, const std::string& value) = 0;
				virtual bool writeValue(const std::string& partition, const std::string& key, int value) = 0;

				/**
				 * @brief reads value by a key
				 * 
				 * @param partition 
				 * @param key 
				 * @param out result 
				 * @return true 
				 * @return false failed in any way to write
				 */

				virtual bool readValue(const std::string& partition, const std::string& key, std::string& out) const = 0;
				virtual bool readValue(const std::string& partition, const std::string& key, int& out) const = 0;

				virtual const char* getName() = 0;
		};
	}
}
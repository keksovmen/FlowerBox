#pragma once



/**
 * @brief interface to read and write
 * 		key value pairs to somewhere
 */



#include <string>

#include "fb_debug.hpp"



namespace fb
{
	namespace storage
	{
		class StorageIface : public debug::Named
		{
			public:
				#ifdef _ESP8266
					using Number = int32_t;
				#else
					using Number = int64_t;
				#endif



				static constexpr int MAX_STRING_SIZE = 256;



				virtual ~StorageIface() = default;

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

				virtual bool hasKey(std::string_view partition, std::string_view key) const = 0;

				/**
				 * @brief writes value by a key
				 * 
				 * @param partition 
				 * @param key 
				 * @param value 
				 * @return true 
				 * @return false failed in any way to write
				 */

				virtual bool writeValue(std::string_view partition, std::string_view key, std::string_view value) = 0;
				virtual bool writeValue(std::string_view partition, std::string_view key, Number value) = 0;

				/**
				 * @brief reads value by a key
				 * 
				 * @param partition 
				 * @param key 
				 * @param out result 
				 * @return true 
				 * @return false failed in any way to write
				 */

				virtual bool readValue(std::string_view partition, std::string_view key, std::string& out) const = 0;
				virtual bool readValue(std::string_view partition, std::string_view key, Number& out) const = 0;

				/**
				 * @brief must delete key value pair, so
				 * hasKey() will return false
				 * 
				 * @param partition 
				 * @param key 
				 */
				virtual void clearValue(std::string_view partition, std::string_view key) = 0;
		};
	}
}
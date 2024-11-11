#pragma once


#include <unordered_map>
#include <utility>

#include "fb_storage.hpp"



namespace fb
{
	namespace storage
	{
		class RamStorage : public StorageIface
		{
			public:

				virtual const char* getName() const override;

				virtual bool init() override;

				virtual bool hasKey(const std::string& partition, const std::string& key) const override;

				virtual bool writeValue(const std::string& partition, const std::string& key, const std::string& value) override;
				virtual bool writeValue(const std::string& partition, const std::string& key, int value) override;

				virtual bool readValue(const std::string& partition, const std::string& key, std::string& out) const override;
				virtual bool readValue(const std::string& partition, const std::string& key, int& out) const override;
			
			private:
				using Entry = std::pair<int, std::string>;
				using Page = std::unordered_map<std::string, Entry>;



				std::unordered_map<std::string, Page> _data;
		};
	}
}
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

				virtual bool hasKey(std::string_view partition, std::string_view key) const override;

				virtual bool writeValue(std::string_view partition, std::string_view key, std::string_view value) override;
				virtual bool writeValue(std::string_view partition, std::string_view key, int64_t value) override;

				virtual bool readValue(std::string_view partition, std::string_view key, std::string& out) const override;
				virtual bool readValue(std::string_view partition, std::string_view key, int64_t& out) const override;
			
			private:
				using Entry = std::pair<int64_t, std::string>;
				using Page = std::unordered_map<std::string, Entry>;



				std::unordered_map<std::string, Page> _data;
		};
	}
}
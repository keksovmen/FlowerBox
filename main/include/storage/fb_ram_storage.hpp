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
				virtual bool writeValue(std::string_view partition, std::string_view key, Number value) override;

				virtual bool readValue(std::string_view partition, std::string_view key, std::string& out) const override;
				virtual bool readValue(std::string_view partition, std::string_view key, Number& out) const override;

				virtual void clearValue(std::string_view partition, std::string_view key) override;
			
			private:
				using Entry = std::pair<Number, std::string>;
				using Page = std::unordered_map<std::string, Entry>;



				std::unordered_map<std::string, Page> _data;



				static bool _contains(const std::unordered_map<std::string, Page>& map, const std::string& v);
				static bool _contains(const Page map, const std::string& v);
				
		};
	}
}
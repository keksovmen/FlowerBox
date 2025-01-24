#pragma once



#include "fb_storage.hpp"



namespace fb
{
	namespace storage
	{
		class NvsStorage : public StorageIface
		{
			public:
				~NvsStorage();

				virtual const char* getName() const override;

				virtual bool init() override;

				virtual bool hasKey(std::string_view partition, std::string_view key) const override;

				virtual bool writeValue(std::string_view partition, std::string_view key, std::string_view value) override;
				virtual bool writeValue(std::string_view partition, std::string_view key, int64_t value) override;

				virtual bool readValue(std::string_view partition, std::string_view key, std::string& out) const override;
				virtual bool readValue(std::string_view partition, std::string_view key, int64_t& out) const override;
		};
	} // namespace nvs
}
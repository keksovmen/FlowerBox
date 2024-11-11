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

				virtual const char* getName() override;

				virtual bool init() override;

				virtual bool hasKey(const std::string& partition, const std::string& key) const override;

				virtual bool writeValue(const std::string& partition, const std::string& key, const std::string& value) override;
				virtual bool writeValue(const std::string& partition, const std::string& key, int value) override;

				virtual bool readValue(const std::string& partition, const std::string& key, std::string& out) const override;
				virtual bool readValue(const std::string& partition, const std::string& key, int& out) const override;
		};
	} // namespace nvs
}
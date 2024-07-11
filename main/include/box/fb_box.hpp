#pragma once



#include <string>

#include "fb_serializable.hpp"
#include "fb_id.hpp"



namespace fb
{
	namespace box
	{
		class Box : public Serializable
		{
			public:
				Box(const std::string& name,
					const std::string& version,
					id::UniqueId uniqueId);


				virtual std::string toJson() const override;
			
			private:
				const id::UniqueId _uniqueId;

				std::string _name;
				std::string _version;
		};
	} // namespace box
	
} // namespace fb

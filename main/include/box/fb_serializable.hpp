#pragma once



#include <string>



namespace fb
{
	namespace box
	{
		class Serializable
		{
			public:
				virtual std::string toJson() const = 0;
		};
	} // namespace box
	
} // namespace fb

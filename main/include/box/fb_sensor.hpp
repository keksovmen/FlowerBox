#pragma once



#include <string>

#include "fb_serializable.hpp"
#include "fb_object_base.hpp"



namespace fb
{
	namespace box
	{
		//TODO: make generic
		//TODO: add list of properties ids
		class Sensor : public ObjectStaticTid
		{
			public:
				Sensor(Tid tid);

				virtual std::string toJson() const override;
		};
	} // namespace box
} // namespace fb

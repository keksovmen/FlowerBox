#pragma once



#include "fb_serializable.hpp"
#include "fb_clock.hpp"



namespace fb
{
	namespace box
	{
		//TODO: made possible to use different types (int, str, etc)
		class DataEntry : public Serializable
		{
			public:
				DataEntry(float value);
				DataEntry(float value, clock::Timestamp timestamp);

				virtual std::string toJson() const override;
			
			private:
				float _value;
				clock::Timestamp _timestamp;

		};
	}
}
#pragma once



#include <string>

#include "fb_serializable.hpp"



namespace fb
{
	namespace box
	{
		class Property : public Serializable
		{
			public:

				Property(const std::string& name,
					const std::string& description,
					const std::string& valueType,
					int id,
					int tid,
					double minValue,
					double maxValue,
					double value);


				virtual std::string toJson() const override;

				int getId() const;
				void setId(int id);
			
			private:
				std::string _name;
				std::string _description;
				std::string _valueType;
				int _id;
				int _tid;
				double _minValue;
				double _maxValue;
				double _value;
		};
	} // namespace box
	
} // namespace fb

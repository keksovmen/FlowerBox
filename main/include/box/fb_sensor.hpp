#pragma once



#include <string>

#include "fb_serializable.hpp"



namespace fb
{
	namespace box
	{
		class Sensor : public Serializable
		{
			public:
				Sensor(const std::string& name,
					const std::string& description,
					const std::string& valueType,
					int id,
					int tid,
					double minValue,
					double maxValue);


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
		};
	} // namespace box
} // namespace fb

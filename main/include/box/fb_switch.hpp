#pragma once



#include <vector>
#include <string>

#include "fb_serializable.hpp"



namespace fb
{
	namespace box
	{
		class Switch : public Serializable
		{
			public:
				Switch(const std::string& name,
					const std::string& description,
					int id,
					int tid,
					bool state,
					std::vector<int> dependentProperties,
					std::vector<int> dependentSensors
					);


				virtual std::string toJson() const override;

				int getId() const;

				void setId(int id);
			
			private:
				std::string _name;
				std::string _description;
				int _id;
				int _tid;
				bool _state;
				std::vector<int> _dependentProperties;
				std::vector<int> _dependentSensors;
		};
	} // namespace box
} // namespace fb

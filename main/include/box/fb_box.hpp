#pragma once



#include <vector>
#include <string>

#include "fb_serializable.hpp"
#include "fb_id.hpp"
#include "fb_property.hpp"
#include "fb_sensor.hpp"



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

				void addProperty(const Property& prop);
				void addSensor(const Sensor& sensor);

				const Property* getProperty(int id) const;
				const Sensor* getSensor(int id) const;
			
			private:
				const id::UniqueId _uniqueId;

				std::string _name;
				std::string _version;

				std::vector<Property> _properties;
				std::vector<Sensor> _sensors;
		};
	} // namespace box
	
} // namespace fb

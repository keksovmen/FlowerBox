#pragma once



#include <memory>
#include <string>
#include <vector>

#include "fb_serializable.hpp"
#include "fb_id.hpp"
#include "fb_property.hpp"
#include "fb_sensor.hpp"
#include "fb_switch.hpp"



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

				void addProperty(std::unique_ptr<PropertyIface> val);
				void addSensor(const Sensor& val);
				void addSwitch(const Switch& val);

				PropertyIface* getProperty(int id);
				const Sensor* getSensor(int id) const;
				const Switch* getSwitch(int id) const;
			
			private:
				const id::UniqueId _uniqueId;

				std::string _name;
				std::string _version;

				std::vector<std::unique_ptr<PropertyIface>> _properties;
				std::vector<Sensor> _sensors;
				std::vector<Switch> _switches;
		};
	} // namespace box
	
} // namespace fb

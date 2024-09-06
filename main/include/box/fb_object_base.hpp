#pragma once



#include <vector>

#include "fb_tid.hpp"
#include "fb_serializable.hpp"



namespace fb
{
	namespace box
	{
		//TODO: just write getters and setters nothing more
		class ObjectIface : public Serializable
		{
			public:
				using PropertyArray = std::vector<int>;



				virtual ~ObjectIface();

				void setTid(Tid tid);
				Tid getTid() const;

				void setId(int id);
				int getId() const;

				void setName(const std::string& val);
				void setDescription(const std::string& val);
				void setValueType(const std::string& val);
				virtual void setMinValueStr(const std::string& val);
				virtual void setMaxValueStr(const std::string& val);

				const std::string& getName() const;
				const std::string& getDescription() const;
				const std::string& getValueType() const;
				const std::string& getMinValueStr() const;
				const std::string& getMaxValueStr() const;

				void addPropertyDependency(int propertyId);
				const PropertyArray& getPropertyDependencies() const;
			
			private:
				Tid _tid;
				int _id = -1;

				std::string _name;
				std::string _description;
				std::string _valueType;
				std::string _minValue;
				std::string _maxValue;

				PropertyArray _dependentProperties;
		};



		//TODO: fetch data from TID mapper and set all fields
		class ObjectStaticTid : public ObjectIface
		{
			public:
				ObjectStaticTid(Tid tid);
				virtual ~ObjectStaticTid();
		};
	}
}
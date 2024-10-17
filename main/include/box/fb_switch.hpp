#pragma once



#include <functional>
#include <string>
#include <vector>

#include "fb_object_base.hpp"



namespace fb
{
	namespace box
	{
		//TODO: make it child of ObjectHolder
		class Switch : public ObjectStaticTid
		{
			public:
				using ReadStateCb = std::function<bool()>;



				Switch(Tid tid,
					std::vector<int> dependentProperties,
					std::vector<int> dependentSensors,
					const ReadStateCb& stateCb);

				virtual std::string toJson() const override;

				void addSensorDependency(int sensorId);
			
			private:
				const ReadStateCb _stateCb;

				std::vector<int> _dependentSensors;
		};
	} // namespace box
} // namespace fb

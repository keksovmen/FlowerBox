#pragma once



#include <functional>

#include "fb_debug.hpp"



namespace fb
{
	namespace sensor
	{
		class SensorIface : public debug::Named
		{
			public:

				virtual ~SensorIface();

				/**
				 * @brief 
				 * 
				 * @return true init was sucess,
				 * 			sensor ready to be called for updates
				 * @return false 
				 */

				bool init();

				/**
				 * @brief reads data from any physical inteface
				 * 
				 * @return true no error detected, sensor status is okay
				 * @return false error detected, and sensor must be called for init()
				 */

				bool update();

				/**
				 * @brief 
				 * 
				 * @return current state 
				 */
				
				bool isInit() const;
			
			private:
				bool _initFlag = false;



				virtual bool _doInit() = 0;
				virtual bool _doUpdate() = 0;
		};
	}
}
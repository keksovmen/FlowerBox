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

				virtual ~SensorIface() = default;

				/**
				 * @brief 
				 * 
				 * @return true init was success,
				 * 			sensor ready to be called for updates
				 * @return false 
				 */

				bool init();

				/**
				 * @brief reads data from any physical inteface, can break sensor to init state
				 * 
				 * @return true value changed
				 * @return false value remained the same
				 */

				bool update();

				/**
				 * @brief 
				 * 
				 * @return current state 
				 */
				
				bool isInit() const;

				virtual float getValue() const;
				virtual float getValueIndexed(int index) const;
				virtual int getIndexCount() const;

			protected:
				enum class UpdateResult : int
				{
					OK,
					VALUE_CHANGED,
					FAIL
				};
				
			private:
				bool _initFlag = false;



				virtual bool _doInit() = 0;
				virtual UpdateResult _doUpdate() = 0;
		};
	}
}
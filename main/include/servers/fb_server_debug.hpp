#pragma once



#include <string_view>

#include "fb_server_builder.hpp"



namespace fb
{
	namespace server
	{
		/**
		 * @brief will transfer log calls to proxy
		 */
		void initServerDebug();
		
		void registerServerDebug(Builder& builder);
	}
}
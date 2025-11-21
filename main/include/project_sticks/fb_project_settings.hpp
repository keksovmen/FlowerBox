#pragma once



#include <string_view>



namespace fb
{
	namespace settings
	{
		//setters

		void setHttpDelay(int delayMs);
		void setHttpUrl(std::string_view url);


		//getters
		int getHttpDelay();
		std::string getHttpUrl();
	}
}
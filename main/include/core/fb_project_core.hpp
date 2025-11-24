#pragma once



namespace fb
{
	namespace project
	{
		struct CoreInfo
		{
			bool requiresTime;
			bool requiresServices = true;
		};



		void init();
		const CoreInfo& getInfo(); 
	}
}
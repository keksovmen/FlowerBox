#pragma once



namespace fb
{
	namespace project
	{
		struct CoreInfo
		{
			bool requiresTime;
		};



		void init();
		const CoreInfo& getInfo(); 
	}
}
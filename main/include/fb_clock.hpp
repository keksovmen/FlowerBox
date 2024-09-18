#pragma once



#include <functional>



namespace fb
{
	namespace clock
	{
		using Timestamp = unsigned long long;



		enum class ClockEventId : int
		{
			SYNCED
		};
		bool operator==(int val, ClockEventId id);



		struct Time
		{
			const int hours;
			const int minutes;
			const int seconds;



			Time(int hours, int minutes, int seconds);

			bool operator<(const Time& t) const;
			operator Timestamp() const;
		};



		void initClock();
		void syncRequest();
		void deinitClock();

		bool isTimeSynced();
		Time getCurrentTime();

		Timestamp currentTimeStamp();
	}
}
#pragma once



#include <stdint.h>
#include <functional>



namespace fb
{
	namespace clock
	{
		using Timestamp = int64_t;



		enum class ClockEventId : int
		{
			SYNCED
		};
		bool operator==(int val, ClockEventId id);



		struct Time
		{
			int hours;
			int minutes;
			int seconds;



			explicit Time(Timestamp secs);
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
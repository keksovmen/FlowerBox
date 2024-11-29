#include <cstdio>
#include <assert.h>



struct Time
{
	int hours;
	int minutes;
	int seconds;

	Time(int hours, int minutes, int seconds) : hours(hours), minutes(minutes), seconds(seconds)
	{
	}

	bool operator<(const Time &t) const
	{
		if (this->hours > t.hours)
		{
			return false;
		}

		if (this->hours < t.hours)
		{
			return true;
		}

		if (this->minutes > t.minutes)
		{
			return false;
		}

		if (this->minutes < t.minutes)
		{
			return true;
		}

		if (this->seconds > t.seconds)
		{
			return false;
		}

		if (this->seconds < t.seconds)
		{
			return true;
		}

		return false;
	}
};




int main()
{
	Time t1(0, 0, 0);
	Time t2(0, 0, 0);

	for(int i = 0; i < 24 * 60 * 60; i++){
		t1.seconds++;
		if(t1.seconds >= 60){
			t1.minutes++;
			if(t1.minutes >= 60){
				t1.hours++;
			}
		}
		t2 = t1;
		t2.seconds--;
		if(!(t2 < t1)){
			printf("%d\n", i);
		}
		assert(t2 < t1);
	}
}
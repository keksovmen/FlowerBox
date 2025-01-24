#include <cassert>
#include <stdint.h>



using Timestamp = int64_t;



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

Time::Time(Timestamp secs)
	: hours(secs / 3600),
		minutes((secs - (hours * 3600)) / 60),
		seconds(secs - (hours * 3600 + minutes * 60))
{

}

Time::Time(int hours, int minutes, int seconds)
	: hours(hours), minutes(minutes), seconds(seconds)
{

}

bool Time::operator<(const Time& t) const
{
	if(this->hours > t.hours){
		return false;
	}

	if(this->hours < t.hours){
		return true;
	}

	if(this->minutes > t.minutes){
		return false;
	}

	if(this->minutes < t.minutes){
		return true;
	}

	if(this->seconds > t.seconds){
		return false;
	}

	if(this->seconds < t.seconds){
		return true;
	}

	return false;
}

Time::operator Timestamp() const
{
	return this->hours * 60 * 60 + this->minutes * 60 + this->seconds;
}

bool isOverflow(Time start, Time end)
{
	return start > end;
}

bool condition(Time start, Time end, Time current)
{
	if(start == end){
		return false;
	}

	if(isOverflow(start, end)){
		return current >= start || current < end;
	}else{
		return current >= start && current < end;
	}
}



static void _test_constructor()
{
	Time t{0};
	assert(t.hours == 0);
	assert(t.minutes == 0);
	assert(t.seconds == 0);

	t = Time{60};
	assert(t.hours == 0);
	assert(t.minutes == 1);
	assert(t.seconds == 0);

	t = Time{3600};
	assert(t.hours == 1);
	assert(t.minutes == 0);
	assert(t.seconds == 0);

	t = Time{3666};
	assert(t.hours == 1);
	assert(t.minutes == 1);
	assert(t.seconds == 6);
}

static void _test_condition()
{
	assert(!isOverflow(Time(0), Time(10)));
	assert(isOverflow(Time(10), Time(0)));

	//after period
	assert(!condition(Time(50), Time(100), Time(160)));
	assert(!condition(Time(10), Time(20), Time(30)));
	//inside period
	assert(condition(Time(50), Time(100), Time(60)));
	//before period
	assert(!condition(Time(50), Time(100), Time(30)));

	//before period
	assert(!condition(Time(100), Time(50), Time(60)));
	//inside period
	assert(condition(Time(100), Time(50), Time(30)));
	//inside period
	assert(condition(Time(100), Time(50), Time(120)));
}



int main()
{
	_test_constructor();
	_test_condition();
}
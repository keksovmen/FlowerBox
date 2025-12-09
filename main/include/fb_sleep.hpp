#pragma once



namespace fb
{
	namespace sleep
	{
		// enum class SleepEvent
		// {
		// 	GOING_SLEEP,
		// 	WAKEUP,
		// };
		// bool operator==(int val, SleepEvent id);




		void enableLightSleep();
		void disableLightSleep();
		void enterDeepSleep(int wakePin, bool isPullUp);
	}
}
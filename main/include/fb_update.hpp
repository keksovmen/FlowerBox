#pragma once



namespace fb
{
	namespace update
	{
		enum class UpdateEventId : int
		{
			START,
			FAILURE,
			END,
		};
		bool operator==(int val, UpdateEventId id);




		bool begin();
		bool writeSequential(const char* data, int size);
		bool end();
	}
}
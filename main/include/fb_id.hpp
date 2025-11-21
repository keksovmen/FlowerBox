#pragma once



namespace fb
{
	namespace id
	{
		#ifdef _ESP8266
			using UniqueId = unsigned long;
		#else
			using UniqueId = unsigned long long;
		#endif
	} // namespace id
	
} // namespace fb

#pragma once



namespace fb
{
	namespace provision
	{
		enum class ProvisionEventId
		{
			SUCCESS,
			FAILURE,
		};
		
		constexpr bool operator==(int val, ProvisionEventId e){
			return val == static_cast<int>(e);
		}



		void init();
		void deinit();
	}
}

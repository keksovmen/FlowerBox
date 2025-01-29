#pragma once



#include <cstdlib>



namespace fb
{
	namespace util
	{
		class DynamicStrConstructor
		{
			public:
				DynamicStrConstructor(char* dynamicStr)
					: _str(dynamicStr)
				{

				}

				~DynamicStrConstructor()
				{
					if(_str){
						free(_str);
					}
				}

				operator char*()
				{
					return _str;
				}
			
			private:
				char* _str;
		};
	}
}
#pragma once



#include <concepts>

#include "esp_log.h"



// #define FB_DEBUG_TAG_ENTER(){ESP_LOGI(TAG, "Entered: %s", __FUNCTION__);}
// #define FB_DEBUG_TAG_EXIT(){ESP_LOGI(TAG, "Exited: %s", __FUNCTION__);}
#define FB_DEBUG_ENTER_I_TAG() auto __proxy = debug::EntryExitProxy(TAG, __FUNCTION__, debug::EntryExitProxy::ColorGreen)
#define FB_DEBUG_ENTER_W_TAG() auto __proxy = debug::EntryExitProxy(TAG, __FUNCTION__, debug::EntryExitProxy::ColorYellow)
#define FB_DEBUG_ENTER_E_TAG() auto __proxy = debug::EntryExitProxy(TAG, __FUNCTION__, debug::EntryExitProxy::ColorRed)

// #define FB_DEBUG_ENTER(){ESP_LOGI(getName(), "Entered: %s", __FUNCTION__);}
// #define FB_DEBUG_EXIT(){ESP_LOGI(getName(), "Exited: %s", __FUNCTION__);}
#define FB_DEBUG_ENTER_I_OBJ() auto __proxy = debug::EntryExitProxy(*this, __FUNCTION__, debug::EntryExitProxy::ColorGreen)
#define FB_DEBUG_ENTER_W_OBJ() auto __proxy = debug::EntryExitProxy(*this, __FUNCTION__, debug::EntryExitProxy::ColorYellow)
#define FB_DEBUG_ENTER_E_OBJ() auto __proxy = debug::EntryExitProxy(*this, __FUNCTION__, debug::EntryExitProxy::ColorRed)

#define FB_DEBUG_LOG_I_TAG(format, ...){ESP_LOGI(TAG, format, ##__VA_ARGS__);}
#define FB_DEBUG_LOG_W_TAG(format, ...){ESP_LOGW(TAG, format, ##__VA_ARGS__);}
#define FB_DEBUG_LOG_E_TAG(format, ...){ESP_LOGE(TAG, format, ##__VA_ARGS__);}

#define FB_DEBUG_LOG_I_OBJ(format, ...){ESP_LOGI(getName(), format, ##__VA_ARGS__);}
#define FB_DEBUG_LOG_W_OBJ(format, ...){ESP_LOGW(getName(), format, ##__VA_ARGS__);}
#define FB_DEBUG_LOG_E_OBJ(format, ...){ESP_LOGE(getName(), format, ##__VA_ARGS__);}



namespace fb
{
	namespace debug
	{
		//TODO: create struct that will call TAG_ENTER in constructor and TAG_EXIT in destructor
		class Named
		{
			public:
				//TODO: make it const
				virtual const char* getName() const = 0;
		};



		class EntryExitProxy
		{
			public:
				using Color = int;



				static constexpr Color ColorGreen = 0;
				static constexpr Color ColorYellow = 1;
				static constexpr Color ColorRed = 2;



				template<class T>
				EntryExitProxy(const T& obj, const char* function, Color col)
					: _color(col)
				{
					static_assert(std::derived_from<T, Named> == true);

					_tag = obj.getName();
					_fName = function;

					_write("Entered");
				}

				EntryExitProxy(const char* tag, const char* function, Color col)
					: _color(col)
				{
					_tag = tag;
					_fName = function;

					_write("Entered");
				}

				~EntryExitProxy()
				{
					_write("Exited");
				}
			
			private:
				const Color _color;
				const char* _tag;
				const char* _fName;



				void _write(const char* msg)
				{
					if(_color == ColorGreen)
					{
						ESP_LOGI(_tag, "%s: %s", msg, _fName);
					}
					else if(_color == ColorYellow)
					{
						ESP_LOGW(_tag, "%s: %s", msg, _fName);
					}
					else
					{
						ESP_LOGE(_tag, "%s: %s", msg, _fName);
					}
				}
		};
	};
}
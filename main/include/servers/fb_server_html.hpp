#pragma once



#include <string_view>

#include "fb_server_builder.hpp"
#include "fb_templates.hpp"



namespace fb
{
	namespace server
	{
		using HtmlFileCb = void (*)(templates::Engine& engine);



		void registerServerHtml(Builder& builder);

		void htmlAddFileHandler(std::string_view fileName, HtmlFileCb fileCb);
	} // namespace server
}
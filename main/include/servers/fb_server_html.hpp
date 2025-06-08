#pragma once



#include <string_view>
#include <unordered_map>

#include "fb_server_builder.hpp"
#include "fb_templates.hpp"



namespace fb
{
	namespace server
	{
		using HtmlFileCb = void (*)(templates::Engine& engine, const std::unordered_map<std::string, std::string>& query);



		void registerServerHtml(Builder& builder);

		void htmlAddFileHandler(std::string_view fileName, HtmlFileCb fileCb);
	} // namespace server
}
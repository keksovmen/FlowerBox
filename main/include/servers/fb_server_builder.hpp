#pragma once



#include <functional>
#include <string>
#include <memory>
#include <vector>

#include "fb_server.hpp"



namespace fb
{
	namespace server
	{
		enum class EndpointMethod : int
		{
			GET = 0,
			POST,
		};



		using EndpointCallback = std::function<bool(httpd_req_t, void*)>;



		struct Endpoint
		{
			std::string address;
			EndpointMethod method;
			EndpointCallback callback;
		};



		class Builder
		{
			public:
				Builder();

				Builder& setPort(int port);
				Builder& addEndpoint(const Endpoint& endpoint);

				std::unique_ptr<Server*> build();
			private:

				int _port;
				std::vector<Endpoint> _endpoints;

		};

	} // namespace serv_builder
}
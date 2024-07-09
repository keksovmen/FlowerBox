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



		using EndpointCallback = esp_err_t(*)(httpd_req_t*);



		struct Endpoint
		{
			std::string address;
			EndpointMethod method;
			void* data;
			EndpointCallback callback;
		};



		class Builder
		{
			public:
				// Builder();

				Builder& setPort(int port);
				Builder& addEndpoint(const Endpoint& endpoint);

				std::unique_ptr<Server> build();
			private:

				int _port = 80;
				std::vector<Endpoint> _endpoints;

		};

	} // namespace serv_builder
}
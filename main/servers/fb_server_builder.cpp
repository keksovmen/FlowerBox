#include "fb_server_builder.hpp"

#include "esp_http_server.h"



using namespace fb;
using namespace server;



Builder& Builder::setPort(int port)
{
	_port = port;

	return *this;
}

Builder& Builder::addEndpoint(const Endpoint& endpoint)
{
	_endpoints.push_back(endpoint);

	return *this;
}

std::unique_ptr<Server> Builder::build()
{
	httpd_handle_t server = nullptr;

	httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
	cfg.max_resp_headers = 32;
	cfg.server_port = _port;
	//turning on /* syntax for uris
	cfg.uri_match_fn = httpd_uri_match_wildcard;
	cfg.max_open_sockets = 1;
	cfg.lru_purge_enable = true;
	cfg.stack_size = 4 * 1024;

	ESP_ERROR_CHECK(httpd_start(&server, &cfg));

	for(auto& point : _endpoints){
		const httpd_uri_t uriHandler = {
			.uri = point.address.c_str(),
			.method = point.method == EndpointMethod::POST ? HTTP_POST : HTTP_GET,
			.handler = point.callback,
			.user_ctx = point.data,
			.is_websocket = point.method == EndpointMethod::WEB_SOCKET,
			.handle_ws_control_frames = true,
			.supported_subprotocol = nullptr,
		};

		httpd_register_uri_handler(server, &uriHandler);
	}

	return std::make_unique<Server>(server);
}

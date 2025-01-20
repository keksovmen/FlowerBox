#include "fb_server.hpp"

#include <memory>

#include "fb_server_builder.hpp"
#include "fb_server_update.hpp"
#include "fb_server_html.hpp"
#include "fb_server_box.hpp"
#include "fb_server_debug.hpp"



using namespace fb;
using namespace server;



static std::unique_ptr<Server> _server;
static std::unique_ptr<Server> _serverDebug;



Server::Server(httpd_handle_t hndl)
	: _hndl(hndl)
{

}

void Server::start()
{
	//TODO: make create new httpd logic from builder
}

void Server::stop()
{
	//TODO: make create new httpd logic from builder
	httpd_stop(_hndl);
}



void server::startProvision()
{
	auto builder = Builder();
	builder.setPort(80);

	registerServerUpdate(builder);
	registerServerHtml(builder);
	registerServerBox(builder);
	// registerServerDebug(builder);

	_server = builder.build();

	builder = Builder();
	builder.setPort(8080);
	builder.setCtrlPort(ESP_HTTPD_DEF_CTRL_PORT + 1);
	registerServerDebug(builder);

	_serverDebug = builder.build();
}

void server::startWorking()
{
	startProvision();
}

void server::stop()
{
	if(_server){
		_server->stop();
		_server.release();
	}

	if(_serverDebug){
		_serverDebug->stop();
		_serverDebug.release();
	}
}

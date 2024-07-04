#pragma once



#include "esp_http_server.h"



namespace fb
{
	namespace server
	{
		class Server
		{
			public:
				Server(httpd_handle_t hndl);

				void start();
				void stop();

			private:
				const httpd_handle_t _hndl;
		};

		
		
		void startInitial();
		void startWorking();

		void stop();
	} // namespace server
	
}
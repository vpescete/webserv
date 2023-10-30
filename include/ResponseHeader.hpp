#ifndef RESPONSE_HEADER_HPP
# define RESPONSE_HEADER_HPP

# include "webserv.h"

class Server;
class RequestHeader;

class ResponseHeader
{
	private:
		Server *server;
		RequestHeader *request;
		Configuration *config; 

	public:
		ResponseHeader(Server *server, RequestHeader *request, Configuration *config);

		~ResponseHeader();
};

#endif

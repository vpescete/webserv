#ifndef RESPONSE_HANDLER_HPP
# define RESPONSE_HANDLER_HPP

# include "webserv.h"

class Server;
class RequestHandler;

class ResponseHandler
{
	private:
		Server	*_server;
		RequestHandler	*_request;
		Configuration	*_config;
		std::map<int, std::string>	_statusCodeMap;
		std::map<std::string, std::string>	_headers;
		void setDefaultHeaders();
		void setStatusCodeMap();
		void setPath();
		void setContent();
		void setContentType(std::string path, std::string type = "");
		void setServer();
		void setStatusCode(const std::string& code);
		void setCookies(const std::string& name, const std::string& value);
		void setContentLenght(const std::string& content);
		void setDate(const std::string& date);
		void setConnection(const std::string& connection);

	public:
		ResponseHandler(Server *_server, RequestHandler *_request, Configuration *_config);
		~ResponseHandler();
		std::string makeResponse() const;
		std::string getResponseCode(int code) const;
		LocationPath getLocationPath(std::string path);
		std::string getDate() const;
};

#endif

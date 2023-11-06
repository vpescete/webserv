#ifndef RESPONSE_HEADER_HPP
# define RESPONSE_HEADER_HPP

# include "webserv.h"

class Server;
class RequestHandler;

class ResponseHeader
{
	private:
		Server	*_server;
		RequestHandler	*_request;
		Configuration	*_config;
		std::map<int, std::string>	_statusCodeMap;
		std::map<std::string, std::string>	_headers;
		void setDefaultHeaders();
		void setStatusCodeMap();
		void pathCreation();
		void contentCreation();
		void contentTypeCreation(std::string path, std::string type = "");
		void setServer();
		void setStatusCode(const std::string& code);
		void setCookies(const std::string& name, const std::string& value);
		void setPath(const std::string& path);
		void setContent(const std::string& content);
		void setContentLenght(const std::string& content);
		void setContentType(const std::string& content_type);
		void setDate(const std::string& date);
		void setConnection(const std::string& connection);

	public:
		ResponseHeader(Server *_server, RequestHandler *_request, Configuration *_config);
		~ResponseHeader();
		std::string makeResponse() const;
		std::string getResponseCode(int code) const;
		LocationPath getLocationPath(std::string path);
		std::string getDate() const;
};

#endif

#ifndef RESPONSE_HEADER_HPP
# define RESPONSE_HEADER_HPP

# include "webserv.h"

class Server;
class RequestHeader;

class ResponseHeader
{
	private:
		Server	*server;
		RequestHeader	*request;
		Configuration	*config;
		std::map<int, std::string>	_statusCodeMap;
		std::map<std::string, std::string>	_headers;
		void setDefaultHeaders();
		void setStatusCodeMap();
		void setStatusCode(const std::string& code);
		void setCookies(const std::string& name, const std::string& value);
		void setContent(const std::string& content);
		void setContentLenght(const std::string& content);
		void setContentType(const std::string& content_type);
		void setDate(const std::string& date);
		void setServer(const std::string& date);
		void setConnection(const std::string& connection);

	public:
		ResponseHeader(Server *server, RequestHeader *request, Configuration *config);
		~ResponseHeader();
		std::string makeResponse(int code) const;
		std::string getResponseCode(int code) const;
		std::string getContentType() const;
		std::string getDate() const;
};

#endif

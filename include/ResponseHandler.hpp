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
		std::map<int, std::string>	_statusCodeMap;
		std::map<std::string, std::string>	_headers;
		std::map<std::string, std::string>	_env;
		std::string _path;
		int _clientSocket;
		void setDefaultHeaders();
		void setStatusCodeMap();
		void setPath(const std::string& requestPath, const std::string& requestMethod);
		void setContent();
		void setContentType(std::string path, std::string type = "");
		void setStatusCode(const std::string& code);
		void setCookies(const std::string& name, const std::string& value);
		void setContentLenght(const std::string& content);
		void setConnection(const std::string& connection);
		bool isDirectory(const std::string& path);
	public:
		ResponseHandler(Server *_server, RequestHandler *_request, int _cs);
		~ResponseHandler();
		std::string makeResponse() const;
		std::string getResponseCode(int code) const;
		LocationPath getLocationPath(std::string path);
		std::string getPath() const;
		std::string getContent() const;
		std::string getContentType() const;
		std::string getContentLenght() const;
		std::string getDate() const;
		std::string getCurrentPath() const;
		std::string getModifyPath(const std::string& requestPath, LocationPath& path);
		std::string getStatusCode() const;
		std::string getErrorPath();
		void	sendResponse();
};

#endif
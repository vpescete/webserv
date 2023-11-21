#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP
# include "webserv.h"

class LocationPath;
class Server;

class RequestHandler {
	private:
		std::string _method;
		std::string _path;
		std::string _protocol;
		std::map <std::string, std::string> _headerMap;
		std::map <std::string, std::string> _mapBody;
		std::vector<std::map<std::string, LocationPath> > _locationPathMap;
	public:
		RequestHandler();
		~RequestHandler();
		void parsereq(std::string buffer, size_t size);
		std::string getMethod();
		std::string getPath();
		std::string getProtocol();
		std::map<std::string, std::string> getHeadersMap();
		void setResponse(Server* svr, int clientSocket, std::string pwd);
		bool autoIndex(int clientSocket);
		std::string getTrueBody();
		std::string extractPath(const std::string& requestLine);
};

#endif

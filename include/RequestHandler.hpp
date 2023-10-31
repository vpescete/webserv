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
		std::map <std::string, std::string> _mapHeader;
		std::string _bodyStart;
		std::string _bodyEnd;
		std::map <std::string, std::string> _mapBody;
		std::vector<std::map<std::string, LocationPath> > _locationPathMap;
		RequestHandler();
	public:
		RequestHandler(std::vector<Server *> srvs);
		~RequestHandler();
		void parsereq(std::string buffer);
		std::string getMethod();
		std::string getPath();
		std::string getProtocol();
		void setResponse(int clientSocket, int index);
		std::string getIndex(int index);
};

#endif
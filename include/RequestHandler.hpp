#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP
# include "webserv.h"

class LocationPath;
class Server;

class RequestHandler {
	private:
		bool		_flag;
		std::string _method;
		std::string _path;
		std::string _protocol;
		std::map <std::string, std::string> _mapHeader;
		std::string _bodyStart;
		std::string _bodyEnd;
		std::string _body;
		std::map <std::string, std::string> _mapBody;
		std::vector<std::map<std::string, LocationPath> > _locationPathMap;
	public:
		RequestHandler();
		~RequestHandler();
		void parsereq(std::string buffer);
		std::string getMethod();
		std::string getPath();
		std::string getProtocol();
		std::string getBody();
		std::map<std::string, std::string> getHeaders();
		void setResponse(Server* svr, int clientSocket, std::string pwd);
		bool autoIndex(int clientSocket);
		std::string getTrueBody();
		std::string get_deleteMethod(std::string::size_type start, std::string::size_type end, std::string temp2, std::string key, std::string value);
		void postMethod(std::string::size_type start, std::string::size_type end, std::string temp2, std::string key, std::string value, const char* headerEnd);
		void uploadNoImage(std::string::size_type start, std::string::size_type end, std::string key, std::string value, const char* headerEnd);
		std::string extractPath(const std::string& requestLine);
		bool getBool();
};

#endif

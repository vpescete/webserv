#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP
# include "webserv.h"

class RequestHandler {
	private:
		std::string _method;
		std::string _path;
		std::string _protocol;
		std::map <std::string, std::string> _mapHeader;
		std::string _bodyStart;
		std::string _bodyEnd;
		std::map <std::string, std::string> _mapBody;
	public:
		RequestHandler();
		~RequestHandler();
		void parsereq(std::string buffer);
		std::string getMethod();
		std::string getPath();
		std::string getProtocol();
		std::string getCookies(const std::string& name);
};

#endif

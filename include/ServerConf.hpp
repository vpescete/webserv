#ifndef SERVERCONF_HPP
# define SERVERCONF_HPP

#include "webserv.h"

class ServerConf {
	private:
		std::ifstream	_confFile;
		std::map<std::string, std::string>	_mapConfiguration;
		std::string		_configurationString;
		std::vector<std::string>	_confVector;
	public:
		// costructor
		ServerConf(const std::string pathFile);
		// ServerConf(const ServerConf & cpy);

		// destructor
		~ServerConf();

		// override operator
		// ServerConf& operator=(const ServerConf & rhs);

		// setter member
		
		// getter member
		std::string	getHost();
		size_t		getPort();

		// member function
		void	confSplitter(std::string confFile);


};

bool	isWhitespace(char c);
void	removeWhitespace(std::string& str);

#endif
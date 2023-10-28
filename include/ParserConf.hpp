#ifndef PARSERCONF_HPP
# define PARSERCONF_HPP

#include "webserv.h"

class Configuration;

class ParserConf {
	private:
		std::ifstream	_confFile;
		std::map<std::string, std::string>	_mapConfiguration;
		std::string		_configurationString;
		std::vector<std::string>	_confVector;
		std::vector<Configuration>	_configurationServer;
		std::map<std::string, std::vector<Configuration> >	_mapConfig;

		// setter member
		void	setMapConfig();
	public:
		// costructor
		ParserConf(const std::string pathFile);
		ParserConf(const ParserConf & cpy);

		// destructor
		~ParserConf();

		// override operator
		ParserConf& operator=(const ParserConf & rhs);
		
		// getter member
		std::string	getServerName();
		std::vector<Configuration>	getConf();
		std::map<std::string, std::vector<Configuration> >	getMapConfig();

		// member function
		void	splitterConfinguration(std::string confFile);
};

bool	isWhitespace(char c);
void	removeWhitespace(std::string& str);

#endif
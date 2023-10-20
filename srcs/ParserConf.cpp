#include "webserv.h"
#include "ParserConf.hpp"

bool isWhitespace(char c) {
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

void removeWhitespace(std::string& str) {
	str.erase(std::remove_if(str.begin(), str.end(), isWhitespace), str.end());
}

ParserConf::ParserConf(std::string const pathFile) : _confFile(pathFile) {
	
	std::stringstream	buf;
	buf << _confFile.rdbuf();
	if (buf.fail()) {
		std::cout << RED << "Error: Invalid path for configuration file" << RESET << std::endl;
		exit(EXIT_FAILURE);
	}
	_configurationString = buf.str();
	splitterConfinguration(_configurationString);
	std::vector<std::string>::iterator it = _confVector.begin();
	for (; it != _confVector.end(); ++it) 
		_configurationServer.push_back(Configuration(*it));

}

ParserConf::ParserConf(const ParserConf & cpy) {
	*this = cpy;
}

ParserConf::~ParserConf() {
}

ParserConf &ParserConf::operator=(const ParserConf &rhs) {
	if (this->_confFile != rhs._confFile)
		*this = rhs;
	return *this;
}

std::string ParserConf::getServerName() {
	try {
		return _mapConfiguration.at("server_name");
	} catch (const std::exception & e) {
		std::cerr << RED << "Server Name not found in the configuration file" << RESET << std::endl;
		exit(EXIT_FAILURE);
	}
}

void ParserConf::splitterConfinguration(std::string confFile) {
	std::string::size_type start;
	std::string::size_type end = 0;

	while (confFile.find('}', 0) != std::string::npos)
	{
		start = confFile.find('{', 0);
		end = confFile.find('}', 0);
		while (confFile[end - 1] != '\n') {
			end = confFile.find('}', end + 1);
		}
		if(end != std::string::npos)
			_confVector.push_back(confFile.substr(start + 2, end - 2));
		confFile = confFile.substr(end + 2);
	}
}

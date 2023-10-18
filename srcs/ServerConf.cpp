#include "webserv.h"

bool isWhitespace(char c) {
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

void removeWhitespace(std::string& str) {
	str.erase(std::remove_if(str.begin(), str.end(), isWhitespace), str.end());
}

ServerConf::ServerConf(std::string const pathFile) : _confFile(pathFile) {
	
	std::stringstream	buf;
	buf << _confFile.rdbuf();
	if (buf.fail()) {
		std::cout << RED << "Error: Invalid path for configuration file" << RESET << std::endl;
		exit(EXIT_FAILURE);
	}
	_configurationString = buf.str();
	confSplitter(_configurationString);
	// std::cout << _configurationString << std::endl;

}

ServerConf::~ServerConf() {
}

std::string ServerConf::getHost() {
	try {
		return _mapConfiguration.at("host");
	} catch (const std::exception & e) {
		std::cerr << RED << "Host not found in the configuration file" << RESET << std::endl;
		exit(EXIT_FAILURE);
	};
}

u_int16_t ServerConf::getPort() {
	try {
		return (u_int16_t)std::atoi(_mapConfiguration.at("port").c_str());
	} catch (const std::exception & e) {
		std::cerr << RED << "Port not found in the configuration file" << RESET << std::endl;
		exit(EXIT_FAILURE);
	};
}

std::string ServerConf::getServerName() {
	try {
		return _mapConfiguration.at("server_name");
	} catch (const std::exception & e) {
		std::cerr << RED << "Server Name not found in the configuration file" << RESET << std::endl;
		exit(EXIT_FAILURE);
	}
}

void ServerConf::confSplitter(std::string confFile)
{
	std::string::size_type start;
	std::string::size_type end = 0;
	std::string key;
	std::string value;

	while (end < confFile.size())
	{
		start = confFile.find('=', 0);
		end = confFile.find(';', 0);
		if(end != std::string::npos) {
			key = confFile.substr(1, start - 1);
			value = confFile.substr(start + 1, end - start - 1);
			removeWhitespace(key);
			_mapConfiguration.insert(std::make_pair<std::string, std::string>(key, value));
			std::cout << RED << key << " : " << GREEN  << value << RESET << std::endl;
		}
		confFile = confFile.substr(end + 1);
	}
}

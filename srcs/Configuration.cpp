#include "webserv.h"
#include "Configuration.hpp"

Configuration::Configuration() {
}

Configuration::Configuration(std::string configString) {
	_confSplitter(configString);
	_setMapLocation();
	getHostPort();
}

Configuration::~Configuration() {
}

void	Configuration::_confSplitter(std::string configString) {
	std::string::size_type start;
	std::string::size_type end = 0;

	while (end < configString.size())
	{
		start = configString.find('=', 0);
		if (configString.substr(1, start - 1) == "location")
		{
			end = configString.find('}', 0);
			while (configString.find('}', end + 1) != std::string::npos)
				end = configString.find('}', end + 1);
			if(end != std::string::npos)
				_map.insert(std::make_pair<std::string, std::string>(
					configString.substr(1, start - 1),
					configString.substr(start + 1, end - start)));
			configString = configString.substr(end, 0);
			if (end != std::string::npos)
				end = configString.find('\n', end);
			else
				end = configString.find('\n', 0);
		}
		else
		{
			end = configString.find('\n', 0);
			if(end != std::string::npos)
				_map.insert(std::make_pair<std::string, std::string>(
					configString.substr(1, start - 1),
					configString.substr(start + 1, end - start - 1)));
			configString = configString.substr(end + 1);
		}
	}
	start = configString.find('=', 0);
	end = configString.find('\0', 0);
	if(start != std::string::npos)
		_map.insert(std::make_pair<std::string, std::string>(
			configString.substr(1, start - 1),
			configString.substr(start + 1, end - 1)));
}

void Configuration::_setMapLocation() {
	if (_map.find("location") == _map.end())
		return;
	std::string stringPath = _map.at("location");
	std::string::size_type start;
	std::string::size_type end = 0;
	std::string::size_type last = 0;
	std::string key;

	do {
		start = stringPath.find('{', 0);
		end = stringPath.find('}', 0);
		last = stringPath.find('/', last);
		key = stringPath.substr(last, stringPath.find('\n', last) - last);
		// std::cout << YELLOW << stringPath << RESET << std::endl;
		_mapLocationPath.insert(std::make_pair<std::string, LocationPath>(key,
			LocationPath(stringPath.substr(start, end - start + 1), key)));
		stringPath = stringPath.substr(end + 1);
		end = stringPath.find('}', 0);
	} while (end < stringPath.size());
}

std::string Configuration::getHost() {
	try {
		std::cout << _map.at("host") << std::endl;
		return _map.at("host");
	} catch (const std::exception & e) {
		std::cerr << RED << "Host not found in the configuration file" << RESET << std::endl;
		exit(EXIT_FAILURE);
	};
}

u_int16_t Configuration::getPort() {
	try {
		std::cout << _map.at("port") << std::endl;
		return (u_int16_t)std::atoi(_map.at("port").c_str());
	} catch (const std::exception & e) {
		std::cerr << RED << "Port not found in the configuration file" << RESET << std::endl;
		exit(EXIT_FAILURE);
	};
}

std::string Configuration::getHostPort() {
	std::string	hostPort;
	hostPort.append(_map["host"]).append(":").append(_map["port"]);
	std::cout << CYAN << hostPort << RESET << std::endl;
	return hostPort;
}

std::map<std::string, LocationPath> Configuration::getLocationPath() {
	return _mapLocationPath;
}

std::string Configuration::getServerName() {
	return _map["server_name"];
}

std::string Configuration::getErrorPath(std::string errorCode) {
	try {
		return _map[errorCode];
	} catch (std::exception & e) {
		return "errors/500.html";
	}
}
#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include "webserv.h"

class LocationPath;

class Configuration {
	private:

		std::map<std::string, std::string> _map;
		std::map<std::string, LocationPath> _mapLocationPath;

		void	_confSplitter(std::string configString);
		void	_setMapLocation();
	public:
		Configuration();
		Configuration(std::string configString);
		~Configuration();

		std::string	getHost();
		u_int16_t	getPort();
		std::string	getHostPort();
		std::map<std::string, LocationPath> getLocationPath();
		std::map<std::string, std::string> getMap();
		std::string	getServerName();
		std::string	getErrorPath(std::string errorCode);

};

#endif

#ifndef LOCATIONPATH_HPP
# define LOCATIONPATH_HPP

# include "webserv.h"

class LocationPath {
	private:
		std::string _locationPath;
		std::map<std::string, std::string> _mapPath;

		void	_setMapPath(std::string confFile);
	public:
		LocationPath();
		LocationPath(std::string confFile, std::string path);

		~LocationPath();

		LocationPath& operator=(const LocationPath & rhs);
		// getter
		std::string	getMethods();
		std::string	getIndex();
		std::string	getUploadDir();
		std::string getRoot();
		std::string getLocationPath();
};

#endif

#include "LocationPath.hpp"

LocationPath::LocationPath() {
}

LocationPath::LocationPath(std::string confFile, std::string path) : _locationPath(path) {
	_setMapPath(confFile);
}

LocationPath::~LocationPath() {
}

LocationPath &LocationPath::operator=(const LocationPath &rhs) {
	if(this->_mapPath != rhs._mapPath)
		*this = rhs;
	return *this;
}

std::string LocationPath::getMethods() {
	return _mapPath["methods"];
}

std::string LocationPath::getIndex() {
	return _mapPath["index"];
}

std::string LocationPath::getUploadDir() {
	return _mapPath["upload_directory"];
}

std::string LocationPath::getRoot() {
	return _mapPath["root"];
}

std::string LocationPath::getLocationPath()
{
	return _locationPath;
}

void LocationPath::_setMapPath(std::string confFile) {
	std::string::size_type start;
	std::string::size_type end = 0;
	std::string::size_type i;
	std::string	key;
	std::string	value;

	confFile = confFile.substr(2, confFile.size() - 2);
	do {
		i = 0;
		while(isspace(confFile[i]))
			i++;
		start = confFile.find('=', 0);
		end = confFile.find('\n', 0);
		if(end != std::string::npos) {
			key = confFile.substr(i, start - i);
			value = confFile.substr(start + 1, end - start - 1);
			_mapPath.insert(std::pair<std::string, std::string>(key, value));
		}
		confFile = confFile.substr(end + 1);
	} while (end < confFile.size());

}

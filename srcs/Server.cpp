#include "webserv.h"
#include "Server.hpp"

Server::Server(Configuration &config) : _conf(&config) {
	_setPort();
	_setHost();
	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_addr.s_addr = inet_addr(_host.c_str());
	_serverAddress.sin_port = htons(_port);
}

Server::~Server() {
}

u_int16_t Server::getPort() {

	return this->_port;
}

std::string Server::getHost() {
	return _host;
}

const sockaddr_in &Server::getServerAddress() const {
	return _serverAddress;
}

void Server::_setPort() {
	_port = _conf->getPort();
}

void Server::_setHost() {
	_host = _conf->getHost();
}

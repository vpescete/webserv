#include "webserv.h"
#include "Server.hpp"

Server::Server(ServerConf &config) : _conf(&config) {
	_setPort();
	_setHost();
}

Server::~Server() {
}

u_int16_t Server::getPort() {

	return this->_port;
}

std::string Server::getHost() {
	return _host;
}

void Server::_setPort() {
	_port = _conf->getPort();
}

void Server::_setHost() {
	_host = _conf->getHost();
}

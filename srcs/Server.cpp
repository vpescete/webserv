#include "webserv.h"
#include "Server.hpp"

Server::Server(ServerConf &config) : _conf(config) {
	_setPort();
	_setHost();
}

Server::~Server() {
}

void Server::_setPort() {
	_port = _conf->getPort();
}

void Server::_setHost() {
	_host = _conf->getHost();
}

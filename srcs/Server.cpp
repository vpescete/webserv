#include "webserv.h"
#include "Server.hpp"

Server::Server(Configuration &config) : _conf(&config) {
	_setPort();
	_setHost();
	this->serverConnection();
	// _serverAddress.sin_addr.s_addr = inet_addr(_host.c_str());
	
}

Server::~Server() {
}

u_int16_t Server::getPort() {

	return this->_port;
}

std::string Server::getHost() {
	return _host;
}

int Server::getSocketFD() {
	return _socketFD;
}

const sockaddr_in &Server::getServerAddress() const {
	return _serverAddress;
}

void Server::SocketException() {
	try {
		if (_socketFD == -1)
			throw std::exception(); 
	} catch (std::exception & e) {
		std::cout << RED << "Error while opening the socket" << RESET << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Server::serverConnection() {
	_socketFD = socket(AF_INET, SOCK_STREAM, 0);
	this->SocketException();
	_serverAddress.sin_family = AF_INET;
	try {
		if ((_serverAddress.sin_addr.s_addr = inet_addr(getHost().c_str())) == INADDR_NONE) {
			throw std::exception();
		}
	} catch (std::exception & e) {
		std::cout << RED << "Host " << getHost() << " is not valid" << RESET << std::endl;
		exit(EXIT_FAILURE);
	}
	int opt = 1;

	// options to let socket reutilize the same port
	if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt))) {
		std::cout << RED << "Error with setsockopt" << RESET << std::endl;
		exit(EXIT_FAILURE); 
	}
	if (setsockopt(_socketFD, SOL_SOCKET, SO_NOSIGPIPE , &opt, sizeof(opt))) {
		std::cout << RED << "Error with setsockopt" << RESET << std::endl;
		exit(EXIT_FAILURE); 
	}
	_serverAddress.sin_port = htons(_port);
	
}

void Server::_setPort() {
	_port = _conf->getPort();
}

void Server::_setHost() {
	_host = _conf->getHost();
}

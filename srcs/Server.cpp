#include "webserv.h"
#include "Server.hpp"

Server::Server(Configuration &config) : _conf(&config) {
	_setPort();
	_setHost();
}

Server::~Server() {
}

Server Server::operator=(Server &rhs) {
	if (this->_host != rhs._host)
		*this = rhs;
	return *this;
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

sockaddr_in *Server::getServerAddress() {
	return &_serverAddress;
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

void Server::serverConnection(int kQueue) {
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
	if (bind(_socketFD, (struct sockaddr*)&_serverAddress, sizeof(_serverAddress)) < 0) {
		std::cout << RED << "Error: Fail to bind port " << getPort() << RESET << std::endl;
		exit(EXIT_FAILURE);
	}
	// Mette il server in ascolto su localhost
	if (listen(_socketFD, 10) < 0) {
		std::cout << RED << "Error: Fail to listet on socket" << RESET << std::endl;
		exit(EXIT_FAILURE);
	}

	// 	struct kevent è un tipo di struttura dati utilizzata in ambienti Unix-like, 
	// in particolare nei sistemi operativi basati su FreeBSD, per gestire eventi di I/O (input/output) asincroni.
	// È parte di un meccanismo noto come "kqueue" (coda kernel), che offre un'efficiente gestione degli eventi di sistema,
	// tra cui eventi di socket, file, segnali, e altro.In un contesto di server web, struct kevent e il sistema kqueue
	// possono essere utilizzati per gestire le operazioni di I/O in modo asincrono ed efficiente, il che è fondamentale per server web ad alte prestazioni.
	EV_SET(&_kevent, _socketFD, EVFILT_READ, EV_ADD, 0, 0, NULL);
	kevent(kQueue, &_kevent, 1, NULL, 0, NULL);
	_setLocationPathMap();
}

void Server::serverDisconnection() {
	close(_socketFD);
}

Configuration *Server::getConf() {
	return _conf;
}

struct kevent Server::getKevent() {
	return _kevent;
}

std::string Server::getIndex() {
	return _locationPathMap.at("/").getIndex();
}

void Server::_setPort() {
	_port = _conf->getPort();
}

void Server::_setHost() {
	_host = _conf->getHost();
}

void Server::_setLocationPathMap() {
	_locationPathMap = getConf()->getLocationPath();
}

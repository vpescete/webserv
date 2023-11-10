#include "Clients.hpp"

Clients::Clients() {
}

Clients::~Clients() {
}

int Clients::addNewClient(int fd, int evIdent) {
	if (fd < 1)
		return -1;
	// int	flags = fcntl(fd, F_GETFL, 0); // la flag F_GETFL fa si di ritornare il numero di flags e identifica le flag utilizzate in modo da poterle cambaire successivamente.
	// assert(flags >= 0);
	// fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	_clientsVect.push_back(client_info(fd, evIdent));
	return 0;
}

Clients::client_info *Clients::getRightConnection(int fd) {
	std::vector<client_info>::iterator it = _clientsVect.begin();
	for ( ; it != _clientsVect.end(); it++) {
		if (fd == (*it).fd)
			return &(*it);
	}
	return 0;
}

int Clients::closeClientConnection(int fd) {
	client_info* id = getRightConnection(fd);
	std::vector<client_info>::iterator it = _clientsVect.begin();
	for (; (*id).fd != (*it).fd; it++);
	close(fd);
	_clientsVect.erase(it);
	return 1;
}

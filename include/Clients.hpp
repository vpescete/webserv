#ifndef CLIENTS_HPP
# define CLIENTS_HPP

#include "webserv.h"

class Clients {
	private:
		struct client_info {
			int	fd;
			int	evIdent;
			client_info(int fd, int evIdent) : fd(fd), evIdent(evIdent) {
			};
		};

	std::vector<client_info> _clientsVect;
	public:
		Clients();
		~Clients();

		int	addNewClient(int fd, int evIdent);
		client_info*	getRightConnection(int fd);
		int	closeClientConnection(int fd);
};

#endif
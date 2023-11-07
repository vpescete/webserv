#include "webserv.h"

bool	running = 1;

const int kQueue = kqueue();

int	getRightSocketFd(std::vector<Server *> srvs, int ident) {
	std::vector<Server *>::iterator it = srvs.begin();
	for (int i = 0; it != srvs.end(); it++, i++) {
		if ((**it).getSocketFD() == ident)
			return i;
	}
	return -1;
}

static void signal_handler(int i) {
	if (i == SIGINT)
		running = 0;
	std::cout << std::endl << YELLOW << "Stopping server..." << RESET << std::endl;
	// exit(0);
}

void	disconnect(std::vector<Server *> srvs) {
	std::vector<Server *>::iterator	it = srvs.begin();
	std::vector<Server *>::iterator	end = srvs.end();
	for (; it != end; it++) {
		(*it)->serverDisconnection();
		delete(*it);
	}
}

std::vector<Server *>	startServer(std::map<std::string, std::vector<Configuration> > mapConfig) {
	std::vector<Server *> srvs;
	std::map<std::string, std::vector<Configuration> >::iterator it = mapConfig.begin();
	for (; it != mapConfig.end(); ++it) {
		Server *s = new Server((*it).second[0]);
		s->serverConnection(kQueue);
		std::cout << CYAN << "Server listening on " << YELLOW << s->getHost() << RESET << ":" << GREEN << s->getPort() << RESET << std::endl;
		srvs.push_back(s);
	}
	return srvs;
}

int	main(int ac, char *av[]) {

	if (ac != 2) {
		std::cout << RED << "ERROR: wrong number of arguments" << RESET <<std::endl;
		return EXIT_FAILURE;
	}
	signal(SIGINT, signal_handler);
	ParserConf confFile(av[1]);

	std::vector<Server *> srvs;
	srvs = startServer(confFile.getMapConfig());
	RequestHandler req;
	char * bufferino = (char *)malloc(100000);
	Clients client;
	struct kevent events[MAXEVENTS];
	ssize_t addrlen = sizeof(sockaddr);
	int clientSocket, bufread = 1;
	int connect;

	while (running) {
		errno = 0;
		int	numEvents = kevent(kQueue, NULL, 0, events, MAXEVENTS, NULL);
		for (int i = 0; i < numEvents; i++) {
			int	index = getRightSocketFd(srvs, events[i].ident);
			if (index != -1) {
				connect = accept(events[i].ident, (struct sockaddr *)(*srvs[index]).getServerAddress(), (socklen_t*)&addrlen);
				printf("%d\n", connect);
				client.addNewClient(connect, events[i].ident);
				EV_SET((*srvs[index]).getKevent(), connect, EVFILT_READ, EV_ADD, 0, 0, NULL);
				write(1, "2\n", 2);
				kevent(kQueue, (*srvs[index]).getKevent(), 1, NULL, 0, NULL);
				write(1, "1\n", 2);
			}
			else if (events[i].filter == EVFILT_READ) {
				if ((bufread = recv(clientSocket, bufferino, 100000, 0)) < 0 ) {
					// if (errno == EAGAIN || errno == EWOULDBLOCK) {
					// 	continue;
					// }
					// else {
					// 	break;
					// }
				}
				req.parsereq(bufferino);
				// autoindex working flawlessy (remember to thank pier also) but the "/autoindex/" below is to be changed based on the configuration file
				if (((req.getMethod() == "GET" && req.getPath().rfind("/autoindex/") != std::string::npos) && req.autoIndex(clientSocket)) || (open(srvs[index]->getIndex().c_str(), O_RDONLY | O_NONBLOCK) == -1)) {
					if (open(srvs[index]->getIndex().c_str(), O_RDONLY | O_NONBLOCK) == -1) {
						req.autoIndex(clientSocket);
					}
					break;
				}
				req.setResponse(srvs[index], clientSocket);
				memset(bufferino, 0, 10000);
				close(clientSocket);
			}
		}
	}
	disconnect(srvs);
	close(kQueue);
	return 0;
}


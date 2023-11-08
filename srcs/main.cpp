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
	// char * bufferino = (char *)malloc(100000);
	Clients client;
	struct kevent events[MAXEVENTS];
	ssize_t addrlen = sizeof(sockaddr);
	int connect;

	while (running) {
		errno = 0;
		std::string bufferStr;
		int	numEvents = kevent(kQueue, NULL, 0, events, MAXEVENTS, NULL);
		for (int i = 0; i < numEvents; i++) {
			int	index = getRightSocketFd(srvs, events[i].ident);
			if (index != -1) {
				// std::cout << CYAN << "Found server with index: " << index << RESET << std::endl;
				connect = accept(events[i].ident, (struct sockaddr *)(*srvs[index]).getServerAddress(), (socklen_t*)&addrlen);
				// printf("%d\n", connect);
				client.addNewClient(connect, events[i].ident);
				EV_SET((*srvs[index]).getKevent(), connect, EVFILT_READ, EV_ADD, 0, 0, NULL);
				kevent(kQueue, (*srvs[index]).getKevent(), 1, NULL, 0, NULL);
			}
			else if (events[i].filter == EVFILT_READ) {
				// std::cout << RED << "Not found server" << RESET << std::endl;
				size_t totalBytesRead = 0;
				int bytesRead = 0;
				index = getRightSocketFd(srvs, client.getRightConnection(events[i].ident)->evIdent);
				char buff[8192];
				do {
					// std::cout << "[DEBUG 1]" << std::endl;
					bytesRead = recv(events[i].ident, buff, 8192, 0);
					// std::cout << "[DEBUG 2]" << std::endl;
					totalBytesRead += bytesRead;
					// std::cout << "[DEBUG 3]" << std::endl;
					if (bytesRead > 0) {
						bufferStr.append(buff, bytesRead);
						// std::cout << "[DEBUG 4]" << std::endl;
					}
					usleep(100);
					// std::cout << "[DEBUG 5]" << std::endl;
				} while (bytesRead > 0);
				req.parsereq(bufferStr);
				std::cout << GREEN << "[DEBUG] " << index << RESET << std::endl;
				// autoindex working flawlessy (remember to thank pier also) but the "/autoindex/" below is to be changed based on the configuration file
				if (((req.getMethod() == "GET" && req.getPath().rfind("/autoindex/") != std::string::npos) && req.autoIndex(events[i].ident)) || (open((*srvs[index]).getIndex().c_str(), O_RDONLY | O_NONBLOCK) == -1)) {
					std::cout << "cacca" << std::endl;
					if (open((*srvs[index]).getIndex().c_str(), O_RDONLY | O_NONBLOCK) == -1) {
						std::cout << (*srvs[index]).getIndex() << std::endl;
						req.autoIndex(events[i].ident);
						req.setResponse(srvs[index], events[i].ident);
					}
					client.closeClientConnection(events[i].ident);
					close(events[i].ident);
					break;
				}
				std::cout << "cacca2" << std::endl;
				req.setResponse(srvs[index], events[i].ident);
				client.closeClientConnection(events[i].ident);
				// memset(bufferino, 0, 10000);
				close(events[i].ident);
			}
		}
	}
	disconnect(srvs);
	close(kQueue);
	return 0;
}


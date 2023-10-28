#include "webserv.h"

bool	running = 1;

const int kQueue = kqueue();

int	getRightSocketFd(std::vector<Server *> servers, int ident) {
	std::vector<Server *>::iterator it = servers.begin();
	for (int i = 0; it != servers.end(); it++, i++) {
		if ((**it).getSocketFD() == ident)
			return i;
	}
	return -1;
}

static void signal_handler(int i) {
	if (i == SIGINT)
		running = 0;
	std::cout << std::endl << YELLOW << "Stopping server..." << RESET << std::endl;
}

void	disconnect(std::vector<Server *> servers) {
	std::vector<Server *>::iterator	it = servers.begin();
	std::vector<Server *>::iterator	end = servers.end();
	for (; it != end; it++) {
		(*it)->serverDisconnection();
		delete(*it);
	}
}

std::vector<Server *>	startServer(std::map<std::string, std::vector<Configuration> > mapConfig) {
	std::vector<Server *> servers;
	std::map<std::string, std::vector<Configuration> >::iterator it = mapConfig.begin();
	for (; it != mapConfig.end(); ++it) {
		Server *s = new Server((*it).second[0]);
		std::cout << CYAN << s->getHost() << RESET << " : " << GREEN << s->getPort() << RESET << std::endl;
		servers.push_back(s);
	}
	return servers;
}

int	main(int ac, char *av[]) {

	if (ac != 2) {
		std::cout << RED << "ERROR: wrong number of arguments" << RESET <<std::endl;
		return EXIT_FAILURE;
	}
	signal(SIGINT, signal_handler);
	ParserConf confFile(av[1]);

	std::vector<Server *> servers;
	servers = startServer(confFile.getMapConfig());
	RequestHandler req;
	// char * bufferino = (char *)malloc(10000);

	struct kevent events[MAXEVENTS];
	while (running) {

		int	numEvents = kevent(kQueue, NULL, 0, events, MAXEVENTS, NULL);
		for (int i = 0; i < numEvents; i++) {
			int	index = getRightSocketFd(servers, events[i].ident);
			if (index != -1) {
				std::cout << "Puerco de: " << index << std::endl;
			}
			else
				std::cout << RED << "Ensooma" << RESET << std::endl;
		}
	}
	disconnect(servers);
	close(kQueue);
	return 0;
}


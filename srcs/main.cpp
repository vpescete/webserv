#include "webserv.h"

bool	running = 1;

const int kQueue = kqueue();

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

std::vector<Server *>	startServer(std::vector<Configuration> vectConfig) {
	std::vector<Server *> servers;
	std::vector<Configuration>::iterator it = vectConfig.begin();
	for (; it != vectConfig.end(); ++it) {
		Server *s = new Server((*it));
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
	servers = startServer(confFile.getConf());
	while (running) {
		;
	}
	disconnect(servers);
	close(kQueue);
	return 0;
}


#include "webserv.h"

bool	running = 1;

static void signal_handler(int i) {
	if (i == SIGINT)
		running = 0;
	std::cout << std::endl << YELLOW << "Stopping server..." << RESET << std::endl;
}

int	main(int ac, char *av[]) {

	if (ac != 2) {
		std::cout << RED << "ERROR: wrong number of arguments" << RESET <<std::endl;
		return EXIT_FAILURE;
	}
	signal(SIGINT, signal_handler);
	ServerConf confFile(av[1]);
	// std::cout << confFile.getHost() << std::endl;
	// std::cout << confFile.getPort() << std::endl;
	Server	svr(confFile);
	while (running)
		;
}


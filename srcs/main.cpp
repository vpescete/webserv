#include "webserv.h"

int	main(int ac, char *av[]) {

	if (ac != 2) {
		std::cout << RED << "ERROR: wrong number of arguments" << RESET <<std::endl;
		return EXIT_FAILURE;
	}
	ServerConf confFile(av[1]);
	std::cout << confFile.getHost() << std::endl;
	std::cout << confFile.getPort() << std::endl;
	// (void)av;
}


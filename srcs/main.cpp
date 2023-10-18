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

	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	// Collega il socket all'indirizzo del server
	bind(serverSocket, (struct sockaddr*)&svr.getServerAddress(), sizeof(svr.getServerAddress()));

	// Mette il server in ascolto su localhost
	listen(serverSocket, 5);

	std::cout << "Server in ascolto su " << BLUE << svr.getHost() << YELLOW << ":" << CYAN << svr.getPort() << RESET << std::endl;
	while (running) {
		int clientSocket = accept(serverSocket, NULL, NULL);
		// Leggi e invia il file index.html
		std::ifstream file("/Users/vpescetelli/webserv/www/index.html");
		if (file.is_open()) {
			std::stringstream buffer;
			buffer << file.rdbuf();
			std::string content = buffer.str();
			std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
			send(clientSocket, response.c_str(), response.length(), 0);
		} else {
			std::string response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
			send(clientSocket, response.c_str(), response.length(), 0);
		}

		// Chiudi la connessione con il client
		close(clientSocket);
	}
	close(serverSocket);
}


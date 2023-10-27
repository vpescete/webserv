#include "webserv.h"
#include <stdlib.h>

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

void fgiulian(int ac, char *av[])
{
  	ServerConf confFile(av[1]);
	Server	svr(confFile);
	RequestHandler req;
	char * bufferino = (char *)malloc(10000);

	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	int opt = 1;

	// options to let socket reutilize the same port
	if (setsockopt(serverSocket, SOL_SOCKET, 
				SO_REUSEADDR , &opt, 
				sizeof(opt))) { 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	}
    if (setsockopt(serverSocket, SOL_SOCKET, 
				SO_NOSIGPIPE , &opt, 
				sizeof(opt))) { 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	}

	// Collega il socket all'indirizzo del server
	bind(serverSocket, (struct sockaddr*)&svr.getServerAddress(), sizeof(svr.getServerAddress()));

	// Mette il server in ascolto su localhost
	listen(serverSocket, 5);
	ssize_t addrlen = sizeof(sockaddr);
	std::cout << "Server in ascolto su " << BLUE << svr.getHost() << YELLOW << ":" << CYAN << svr.getPort() << RESET << std::endl;
	while (running) {
		int clientSocket = accept(serverSocket, (struct sockaddr*) &svr.getServerAddress(), (socklen_t*)&addrlen);
		// read the request from the client
		if (int bufread = recv(clientSocket, bufferino, 10000, 0) < 0)
		{
			break;
		}
		//printf("%s\n", bufferino);
		//call to parse the request in RequestHandler class
		req.parsereq(bufferino);
		// std::cout << req.getMethod() << std::endl << req.getPath() << std::endl << req.getProtocol() << std::endl;
		// Leggi e invia il file index.html di default al primo caricamento della pagina
		if (req.getPath() == "/") {
			std::ifstream file("./www/index.html");
			if (file.is_open()) {
				std::stringstream buffer;
				buffer << file.rdbuf();
				std::string content = buffer.str();
				std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
				send(clientSocket, response.c_str(), response.length(), 0);
				
			} else {
				std::ifstream file("./errors/404.html");
				std::stringstream buffer;
				buffer << file.rdbuf();
				std::string content = buffer.str();
				// std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
				// send(clientSocket, response.c_str(), response.length(), 0);
				std::string response = "HTTP/1.1 404 Not Found\r\nContent-Length:  " + std::to_string(content.length()) + "\r\n\r\n" + content;
				send(clientSocket, response.c_str(), response.length(), 0);
			}
		}

		// Read and send the file requested from the path in the request
		else {
			std::ifstream file("." + req.getPath());
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
		}
		std::cout << req.getPath()<< std::endl;
		memset(bufferino, 0, 10000);
		// Chiudi la connessione con il client
		close(clientSocket);
	}
	close(serverSocket);
	bufferino = NULL;
	free(bufferino);
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


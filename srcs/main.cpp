#include "webserv.h"

bool	running = 1;

const int kQueue = kqueue();

int	getRightSocketFd(std::vector<Server *> diocane, int ident) {
	std::vector<Server *>::iterator it = diocane.begin();
	for (int i = 0; it != diocane.end(); it++, i++) {
		// std::cout << (**it).getSocketFD() << " : " << ident << std::endl;
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

void	disconnect(std::vector<Server *> diocane) {
	std::vector<Server *>::iterator	it = diocane.begin();
	std::vector<Server *>::iterator	end = diocane.end();
	for (; it != end; it++) {
		(*it)->serverDisconnection();
		delete(*it);
	}
}

std::vector<Server *>	startServer(std::map<std::string, std::vector<Configuration> > mapConfig) {
	std::vector<Server *> diocane;
	std::map<std::string, std::vector<Configuration> >::iterator it = mapConfig.begin();
	for (; it != mapConfig.end(); ++it) {
		Server *s = new Server((*it).second[0]);
		s->serverConnection(kQueue);
		// std::cout << CYAN << s->getHost() << RESET << " : " << GREEN << s->getPort() << RESET << std::endl;
		diocane.push_back(s);
	}
	return diocane;
}

int	main(int ac, char *av[]) {

	if (ac != 2) {
		std::cout << RED << "ERROR: wrong number of arguments" << RESET <<std::endl;
		return EXIT_FAILURE;
	}
	signal(SIGINT, signal_handler);
	ParserConf confFile(av[1]);

	std::vector<Server *> diocane;
	diocane = startServer(confFile.getMapConfig());
	RequestHandler req;
	char * bufferino = (char *)malloc(10000);

	struct kevent events[MAXEVENTS];
	ssize_t addrlen = sizeof(sockaddr);
	while (running) {

		int	numEvents = kevent(kQueue, NULL, 0, events, MAXEVENTS, NULL);
		// std::cout << RED << numEvents << RESET << std::endl;
		// sleep(10000);
		for (int i = 0; i < numEvents; i++) {
			int	index = getRightSocketFd(diocane, events[i].ident);
			if (index != -1) {
				int clientSocket = accept(diocane[index]->getSocketFD(), (struct sockaddr *)(*diocane[index]).getServerAddress(), (socklen_t*)&addrlen);
				if (int bufread = recv(clientSocket, bufferino, 10000, 0) < 0) {
					break;
				}
				req.parsereq(bufferino);
				if (req.getPath() == "/") {
					std::ifstream file(diocane[index]->getIndex());
					if (file.is_open()) {
						std::stringstream buffer;
						buffer << file.rdbuf();
						std::string content = buffer.str();
						std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
						send(clientSocket, response.c_str(), response.length(), 0);
						
					}
					else {
						std::ifstream file("./errors/404.html");
						std::stringstream buffer;
						buffer << file.rdbuf();
						std::string content = buffer.str();
						// std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
						// send(clientSocket, response.c_str(), response.length(), 0);
						std::string response = "HTTP/1.1 404 Not Found\r\nContent-Length:  " + std::to_string(content.length()) + "\r\n\r\n" + content;
						send(clientSocket, response.c_str(), response.length(), 0);
					}
					// Read and send the file requested from the path in the request
				}
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
				// std::cout << "Puerco de: " << index << std::endl;
			}
			else
				std::cout << RED << getRightSocketFd(diocane, events[i].ident) << RESET << std::endl;
		}
	}
	disconnect(diocane);
	close(kQueue);
	return 0;
}


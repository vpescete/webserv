#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.h"

class ParserConf;
class Configuration;

class Server {
	private:
		u_int16_t		_port;
		std::string		_host;
		int				_socketFD;
		Configuration	*_conf;

		// setter member
		void	_setPort();
		void	_setHost();

		// socket
		struct sockaddr_in _serverAddress;
		struct kevent		_kevent;
	public:
		Server(Configuration& config);
		// Server(Server & cpy);
		~Server();

		// Server operator=(Server & rhs);

		// getter
		u_int16_t	getPort();
		std::string	getHost();
		int			getSocketFD();
		Configuration&	getConf();
		struct sockaddr_in* getServerAddress();
		void		SocketException();
		void		serverConnection(int kQueue);
		void		serverDisconnection();
};

#endif

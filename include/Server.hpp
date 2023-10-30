#ifndef GREATSERVER_HPP
# define GREATSERVER_HPP

#include "webserv.h"

class ParserConf;
class Configuration;
class LocationPath;

class Server {
	private:
		u_int16_t		_port;
		std::string		_host;
		int				_socketFD;
		Configuration	*_conf;
		std::map<std::string, LocationPath> _locationPathMap;

		// setter member
		void	_setPort();
		void	_setHost();
		void	_setLocationPathMap();

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
		struct sockaddr_in* getServerAddress();
		void		SocketException();
		void		serverConnection(int kQueue);
		void		serverDisconnection();
		Configuration	*getConf();
		std::string	getIndex();
};

#endif
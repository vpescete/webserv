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
		std::map<std::string, std::string> _map;
		std::map<std::string, LocationPath> _locationPathMap;

		// setter member
		void	_setPort();
		void	_setHost();
		void	_setMap();
		void	_setLocationPathMap();

		// socket
		struct sockaddr_in _serverAddress;
		struct kevent		_kevent;
	public:
		Server(Configuration& config);
		// Server(Server & cpy);
		~Server();

		Server operator=(Server & rhs);

		// getter
		u_int16_t	getPort();
		std::string	getHost();
		int			getSocketFD();
		Configuration&	getConf();
		struct sockaddr_in* getServerAddress();
		void		SocketException();
		void		serverConnection(int kQueue);
		void		serverDisconnection();
		Configuration	*getConf2();
		struct kevent*	getKevent();
		std::string		getIndex();
		std::map<std::string, LocationPath> getLocationPathMap();
		std::map<std::string, std::string> getMap();
};

#endif

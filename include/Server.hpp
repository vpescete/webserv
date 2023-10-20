#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.h"

class ParserConf;
class Configuration;

class Server {
	private:
		u_int16_t		_port;
		std::string	_host;
		Configuration	*_conf;

		// setter member
		void	_setPort();
		void	_setHost();

		// socket
		struct sockaddr_in _serverAddress;
	public:
		Server(Configuration & config);
		// Server(Server & cpy);
		~Server();

		// Server operator=(Server & rhs);

		// getter
		u_int16_t	getPort();
		std::string	getHost();
		const struct sockaddr_in& getServerAddress() const;

};

#endif
#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.h"

class ServerConf;

class Server {
	private:
		u_int16_t		_port;
		std::string	_host;
		ServerConf	*_conf;

		// setter member
		void	_setPort();
		void	_setHost();
	public:
		Server(ServerConf & config);
		// Server(Server & cpy);
		~Server();

		// Server operator=(Server & rhs);

		// getter
		u_int16_t	getPort();
		std::string	getHost();

		
};

#endif
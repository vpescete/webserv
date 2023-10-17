#ifndef WEBSERV_H
# define WEBSERV_H


// ANSI Colors
# define RED "\e[0;31m"
# define GREEN "\e[0;32m"
# define YELLOW "\e[0;33m"
# define BLUE "\e[0;34m"
# define MAGENTA "\e[0;35m"
# define CYAN "\e[0;36m"
# define RESET "\e[0m"

// MACRO 
# define DEFAULT_PATH "conf/server.conf"


// C Library
# include <ctype.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <sys/event.h>
# include <sys/stat.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <stdlib.h>


// C++ Library
# include <iostream>
# include <cstdlib>
# include <iomanip>
# include <ctime>
# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include <vector>
# include <map>
# include <algorithm>


// class
# include "ServerConf.hpp"
# include "Server.hpp"

#endif
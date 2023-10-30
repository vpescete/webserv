#ifndef WEBSERV_H
# define WEBSERV_H

// ANSI Colors
# define RED "\e[0;31m"
# define GREEN "\e[0;32m"
# define YELLOW "\e[93;5;226m"
# define BLUE "\e[0;34m"
# define MAGENTA "\e[0;35m"
# define CYAN "\e[0;36m"
# define RESET "\e[0m"

// MACRO 
# define DEFAULT_PATH "conf/server.conf"
# define DEFAULT_ERROR_PAGE "errors/500.html"
# define MAXEVENTS 10

// ERROR PAGES MACRO
# define E_400 "error_page_400"
# define E_403 "error_page_403"
# define E_404 "error_page_404"
# define E_405 "error_page_405"
# define E_406 "error_page_406"
# define E_413 "error_page_413"
# define E_500 "error_page_500"

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
# include "ParserConf.hpp"
# include "Server.hpp"
# include "Configuration.hpp"
# include "LocationPath.hpp"
# include "RequestHandler.hpp"

#endif
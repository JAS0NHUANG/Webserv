#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <string.h> // strerror
#include <errno.h> // errno
#include <unistd.h> //close
#include <stdlib.h> //exit
#include <fcntl.h> //fcntl
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include <arpa/inet.h>

// containers
#include <map>
#include <set>
#include <vector>
#include <queue>

// Classes
class Server;
#include "Location.hpp"
#include "Server.hpp"
#include "Socket.hpp"

#include "config.hpp"
#include "utils.hpp"
#include "colorcodes.hpp"

#endif
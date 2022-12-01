#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <csignal>

#include <arpa/inet.h>
#include <sys/epoll.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>

// containers
#include <map>
#include <set>
#include <vector>
#include <queue>

// Classes
#include "Location.hpp"
#include "Config.hpp"
#include "Socket.hpp"
#include "Client.hpp"

#include "config_parsing.hpp"
#include "utils.hpp"
#include "colorcodes.hpp"

#define BUFFER_SIZE 2056
#define MAX_EVENTS	128

extern std::sig_atomic_t g_shutdown ;

#endif

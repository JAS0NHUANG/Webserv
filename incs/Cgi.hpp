#ifndef CGI_HPP
# define CGI_HPP
#include <unistd.h> //fork pipe
#include <iostream>
#include <string>
#include <fcntl.h>
#include "webserv.hpp"
#include "Client.hpp"

class Cgi{
    public:
        Cgi(Client &requ, Config conf, Location location);
		~Cgi(void);      
        std::pair<bool, std::string>   handler(char *filename);


    private:
        std::map<std::string, std::string>	env;
        char								**_env_char;
        void								set_env();
        void								env_to_char(void);
        Client								_client;
		Location							_location;
		Config								_conf;
};
#endif

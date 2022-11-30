#ifndef CGI_HPP
# define CGI_HPP
#include <unistd.h> //fork pipe
#include <iostream>
#include <string>
#include <fcntl.h>
#include "incs/webserv.hpp"

class Cgi{
    public:
        Cgi(Client &requ, Config &config);	
		~Cgi(void);      
        std::pair<bool, std::string>   handler(char *filename);


    private:
        std::map<std::string, std::string>	env;
        char **env_char;
        void    set_env(Client &requ, Config &config);
        void    env_to_char(void);
        //char 		*arg[];
};
#endif
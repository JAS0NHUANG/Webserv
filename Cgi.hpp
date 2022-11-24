#ifndef CGI_HPP
# define CGI_HPP
#include <unistd.h> //fork pipe
#include <iostream>
#include <string>
#include <fcntl.h>
#include "incs/webserv.hpp"
//#include ""
struct Request {
	int 								method;
	std::string							path;
	bool								isHttp1_1;
	std::map<std::string, std::string>	headers;
	std::string							body;
};
struct Request;

class Server;

class Cgi{
    public:
        Cgi(Client &requ, Config &config);	
		~Cgi(void);      
        void    set_env(Client &requ, Config &config);
        void    env_to_char(void);
        std::string    handler(char *filename);


    private:
        std::map<std::string, std::string>	env;
        char **env_char;
        //char 		*arg[];
};
#endif
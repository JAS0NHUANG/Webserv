#include "Cgi.hpp"
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include "incs/webserv.hpp"

//ref:http://www.wijata.com/cgi/cgispec.html#6.0
Cgi::Cgi(Client &requ, Config &config){
    this->env_char = NULL;
    set_env(requ, config);
    env_to_char();
    
}

Cgi::~Cgi(void){
    if (this->env_char){
        for (int i = 0; this->env_char[i]; i++)
		    delete[] this->env_char[i];
	    delete[] this->env_char;
    }
    //std::cerr<< "delete cgi" << "\n";
}

void print_env(std::map<std::string, std::string>	env){
    for( std::map<std::string, std::string>::iterator it=env.begin(); it!=env.end(); ++it){
        std::cerr << it->first + "=" + it->second << "\n";
    }  
}

void Cgi::set_env(Client &requ, Config &config){
    std::map<std::string, std::string>	headers = requ.get_headers();
    if (config.get_location(requ.get_request_target()).first == true){
        Location location = config.get_location(requ.get_request_target()).second;
        this->env["CONTENT_LENGTH"] = toString(requ.get_body().length());
        this->env["CONTENT_TYPE"] = headers["Content-Type"];
        this->env["CONTENT"] = requ.get_body();
        this->env["GATEWAY_INTERFAC"] = "CGI/1.1";
        this->env["SERVER_PROTOCOL"]= "HTTP/1.1";
        this->env["PATH_INFO"] = requ.get_request_target();
        std::cerr << "requ.method:|" << requ.get_method() <<"|\n";
        this->env["REQUEST_METHOD"]= requ.get_method();
        this->env["QUERY_STRING"] = requ.get_query_string();
        this->env["REMOTE_HOST"]= headers["Host"];
        //REMOTE_ADDR
        this->env["SCRIPT_FILENAME"] = location.get_root() + requ.get_request_target();
        //!!!!!!!!!!!!!!!!!
        this->env["SERVER_NAME"] = config.get_server_name()[0];
        this->env["SERVER_PORT"] = toString(config.get_port());
        this->env["SERVER_SOFTWARE"]= "WEBSERV/1.1";
        this->env["REDIRECT_STATUS"]="200"; //php
        print_env(this->env);
        std::cerr << "env_size" << this->env.size() << "\n";
    }else{
        errMsgErrno("cgi location is not valid");
        return ;
    }
    

}

void close_fd(int fd_in[], int fd_out[]){
        close(fd_in[1]);
	 	close(fd_in[0]);
        close(fd_out[1]);
        close(fd_out[0]);
}

void Cgi::env_to_char(){

    this->env_char = new char*[this->env.size() + 1];
    int i = 0;
    for( std::map<std::string, std::string>::iterator it=this->env.begin(); it!=this->env.end(); ++it){
        std::string item = it->first + "=" + it->second;
        this->env_char[i] = new char[item.size() + 1];
        strcpy(this->env_char[i], item.c_str());
        i++;
    }
    this->env_char[i] = NULL;
}

std::string Cgi::handler(char * cgi_script){
 
    int		fd_in[2];
    int     fd_out[2];
    pid_t	pid;
    //std::string content ="";
    char 		*arg[] = {0};

	if (pipe(fd_out) < 0 || pipe(fd_in) < 0){
        std::cerr << "pipe:" << strerror(errno) << std::endl;
        return NULL;
    }
	if (write(fd_in[1], const_cast<char*>(this->env["CONTENT"].c_str()), this->env["CONTENT"].length() + 1) < 0){
        std::cerr << "write in cgi:" << strerror(errno) << std::endl;
        close_fd(fd_in, fd_out);
        return NULL;
    }
	if ((pid = fork()) < 0){
        std::cerr << "fork:" << strerror(errno) << std::endl;
        close_fd(fd_in, fd_out);
        return NULL;
    }
	else if (pid == 0)
	{
		if (dup2(fd_in[0], STDIN_FILENO)< 0){
            std::cerr << "dup2 in in cgi:" << strerror(errno) << std::endl;
            close_fd(fd_in, fd_out);
			return NULL;
        }
		close(fd_in[1]);
	 	close(fd_in[0]);
		if (dup2(fd_out[1], STDOUT_FILENO)< 0){
            std::cerr << "dup2 out in cgi:" << strerror(errno) << std::endl;
            close_fd(fd_in, fd_out);
			return NULL;
        }
		close(fd_out[1]);
	 	close(fd_out[0]);
		execve(cgi_script, arg, this->env_char);
		std::cerr << "execve in cgi:" << strerror(errno) << std::endl;
    }
	close(fd_in[1]);
	close(fd_in[0]);
    close(fd_out[1]);
    int status;
    waitpid(-1, &status, 0);
	char buff[1024] = {0};
	std::string body;
	ssize_t res = 0;
	while ((res = read(fd_out[0], buff, 1024)) > 0)
	{
		
		buff[res] = '\0';
		body += buff;
		std::cerr << "RES:" << res << "|, body" << body << "\n" ;
	}
	close(fd_out[0]);
    return body;
}
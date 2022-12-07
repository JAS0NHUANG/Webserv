#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include "Cgi.hpp"

//ref:http://www.wijata.com/cgi/cgispec.html#6.0
Cgi::Cgi(Client &requ, Config &config){
    this->env_char = NULL;
    this->request =  requ;
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
        if (requ.get_method() == "POST"){
            this->env["CONTENT_LENGTH"] = toString(requ.get_body().size());
            this->env["CONTENT_TYPE"] = headers["content-type"].substr(1);
        } else if (requ.get_method() == "GET")
            this->env["QUERY_STRING"] = requ.get_query_string();
        this->env["CONTENT"] = requ.get_body();
        this->env["GATEWAY_INTERFAC"] = "CGI/1.1";
        this->env["SERVER_PROTOCOL"]= "HTTP/1.1";
        this->env["PATH_INFO"] = requ.get_request_target();
        std::cerr << "requ.method:|" << requ.get_method() <<"|\n";
        this->env["REQUEST_METHOD"]= requ.get_method();
        this->env["REMOTE_HOST"]= headers["Host"];
        //REMOTE_ADDR
        this->env["SCRIPT_FILENAME"] = location.get_root() + requ.get_request_target();
        //!!!!!!!!!!!!!!!!!
        this->env["SERVER_NAME"] = config.get_server_name()[0];
        this->env["SERVER_PORT"] = toString(config.get_port());
        this->env["SERVER_SOFTWARE"]= "WEBSERV/1.1";
        this->env["REDIRECT_STATUS"]="200"; //php
        print_env(this->env);
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

std::pair<bool, std::string> Cgi::handler(char * cgi_script){
 
    int		fd_in[2];
    int     fd_out[2];
    pid_t	pid;
    //std::string content ="";
    char 		*arg[] = {
        cgi_script,
        const_cast<char*>(this->env["SCRIPT_FILENAME"].c_str()),
        NULL
    };
    std::string body;
    std::string get_request_body;
    std::cerr << "cgi_script: |" << cgi_script << "|\n";
    if (!this->request.get_body().empty()){
        get_request_body = this->request.get_body().substr(2);
    }
	if (pipe(fd_out) < 0 || pipe(fd_in) < 0){
        std::cerr << "pipe: cgi"  << std::endl;
        return std::make_pair(false, body);
    }
    std::cerr << "this->env[\"CONTENT\"]:|" << get_request_body << "|\n";
    if (write(fd_in[1], get_request_body.c_str(), get_request_body.size()) < 0){
        std::cerr << "write in cgi"  << std::endl;
        close_fd(fd_in, fd_out);
        return std::make_pair(false, body);
    }
	if ((pid = fork()) < 0){
        std::cerr << "fork: cgi" << std::endl;
        close_fd(fd_in, fd_out);
        return std::make_pair(false, body);
    }else if (pid == 0){
        close(fd_in[1]);
		if (dup2(fd_in[0], STDIN_FILENO)< 0){
            std::cerr << "dup2 in in cgi"  << std::endl;
            close_fd(fd_in, fd_out);
			return std::make_pair(false, body);
        }
	 	close(fd_in[0]);
        close(fd_out[0]);
		if (dup2(fd_out[1], STDOUT_FILENO)< 0){
            std::cerr << "dup2 out in cgi" << std::endl;
            close_fd(fd_in, fd_out);
			return std::make_pair(false, body);
        }
		close(fd_out[1]);
		execve(cgi_script, arg, this->env_char);
		std::cerr << "execve in cgi"<< std::endl;
    }
    close(fd_in[1]);
	close(fd_in[0]);
    close(fd_out[1]);
    int status;
    waitpid(-1, &status, 0);
    
	char buff[1024] = {0};
	ssize_t res = 0;
	while ((res = read(fd_out[0], buff, 1024)) > 0)
	{
		
		buff[res] = '\0';
		body += buff;
        std::cerr << "RES000:" << res << "|, body" << body << "|\n" ;
	}
    std::cerr << "RES:" << res << "|, body" << body << "|\n" ;
    std::size_t found = body.find("\r\n\r\n");
    if (found!=std::string::npos){
        body = body.substr(found);
    }
	close(fd_out[0]);
    return std::make_pair(true, body);
}
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include "Cgi.hpp"

//ref:http://www.wijata.com/cgi/cgispec.html#6.0
Cgi::Cgi(Client &client, Config conf, Location location){
	_conf = conf;
	_location = location;
    this->_env_char = NULL;
    this->_client = client;
    set_env();
    env_to_char();
}

Cgi::~Cgi(void){
    if (this->_env_char){
        for (int i = 0; this->_env_char[i]; i++)
		    delete[] this->_env_char[i];
	    delete[] this->_env_char;
    }
}

void print_env(std::map<std::string, std::string>	env){
    for( std::map<std::string, std::string>::iterator it=env.begin(); it!=env.end(); ++it){
        std::cerr << it->first + "=" + it->second << "\n";
    }
}

void Cgi::set_env(){
	if (_client.get_method() == "POST") {
		this->env["CONTENT_LENGTH"] = to_String(_client.get_body().size());
		this->env["CONTENT_TYPE"] = _client.get_headers()["content-type"].substr(1);
	}
	else if (_client.get_method() == "GET")
		this->env["QUERY_STRING"] = _client.get_query_string();
	this->env["CONTENT"] = _client.get_body();
	this->env["GATEWAY_INTERFAC"] = "CGI/1.1";
	this->env["SERVER_PROTOCOL"]= "HTTP/1.1";
	this->env["PATH_INFO"] = _client.get_request_target();
	this->env["REQUEST_METHOD"]= _client.get_method();
	this->env["REMOTE_HOST"]= _client.get_headers()["Host"];
	this->env["SCRIPT_FILENAME"] = _client.get_path();
	this->env["SERVER_NAME"] = _client.get_server_name();
	this->env["SERVER_PORT"] = to_String(_conf.get_port());
	this->env["SERVER_SOFTWARE"]= "WEBSERV/1.1";
	this->env["REDIRECT_STATUS"]="200";
    // print_env(this->env);
}

void close_fd(int fd_in[], int fd_out[]){
        close(fd_in[1]);
	 	close(fd_in[0]);
        close(fd_out[1]);
        close(fd_out[0]);
}

void Cgi::env_to_char(){

    this->_env_char = new char*[this->env.size() + 1];
    int i = 0;
    for( std::map<std::string, std::string>::iterator it=this->env.begin(); it!=this->env.end(); ++it){
        std::string item = it->first + "=" + it->second;
        this->_env_char[i] = new char[item.size() + 1];
        strcpy(this->_env_char[i], item.c_str());
        i++;
    }
    this->_env_char[i] = NULL;
}

std::pair<bool, std::string> Cgi::handler(char * cgi_script){
 
    int		fd_in[2];
    int     fd_out[2];
    pid_t	pid;

    char 		*arg[] = {
        cgi_script,
        const_cast<char*>(this->env["SCRIPT_FILENAME"].c_str()),
        NULL
    };
    std::string body;
    std::string get_request_body;
    if (!this->_client.get_body().empty()){
        get_request_body = this->_client.get_body();
    }
	if (pipe(fd_out) < 0 || pipe(fd_in) < 0){
		_client.log(_client.log_error("pipe()"), false);
        return std::make_pair(false, body);
    }
    if (write(fd_in[1], get_request_body.c_str(), get_request_body.size()) < 0){
		_client.log(_client.log_error("write()"), false);
        close_fd(fd_in, fd_out);
        return std::make_pair(false, body);
    }
	if ((pid = fork()) < 0){
		_client.log(_client.log_error("fork()"), false);
        close_fd(fd_in, fd_out);
        return std::make_pair(false, body);
    }else if (pid == 0){
        close(fd_in[1]);
		if (dup2(fd_in[0], STDIN_FILENO) < 0){
            close_fd(fd_in, fd_out);
			exit(40);
        }
	 	close(fd_in[0]);
        close(fd_out[0]);
		if (dup2(fd_out[1], STDOUT_FILENO) < 0){
            close_fd(fd_in, fd_out);
			exit(41);
        }
		close(fd_out[1]);
		if (execve(cgi_script, arg, this->_env_char) < 0) {
			close_fd(fd_in, fd_out);
			exit(42);
		}
    }
    close(fd_in[1]);
	close(fd_in[0]);
    close(fd_out[1]);

    int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status)) {
		status = WEXITSTATUS(status);
		if (status >= 40 && status <= 42) {
			if (status == 40)
				_client.log(_client.log_error("dup2()"), false);
			if (status == 41)
				_client.log(_client.log_error("dup2()"), false);
			if (status == 42)
				_client.log(_client.log_error("execve()"), false);
			return std::make_pair(false, body);
		}
	}

	char buff[1024] = {0};
	ssize_t res = 0;
	if ((res = read(fd_out[0], buff, 1024)) < 0) {
		close(fd_out[0]);
		return std::make_pair(false, body);
	}
	while (res > 0)
	{
		buff[res] = '\0';
		body += buff;
		res = read(fd_out[0], buff, 1024);
	}
    std::size_t found = body.find("\r\n\r\n");
    if (found != std::string::npos) {
        body = body.substr(found);
    }
	close(fd_out[0]);
    return std::make_pair(true, body);
}

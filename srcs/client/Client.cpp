#include "Client.hpp"

Client::Client() {}

Client::Client(int fd, std::vector<Config> virtual_servers) :
	_status_code(0),
	_process_request_line(true),
	_process_headers(true),
	_process_body(true),
	_request_is_complete(false),
	_timeout(std::time(NULL)),
	_fd(fd),
	_virtual_servers(virtual_servers) {
	char ip[INET_ADDRSTRLEN];
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);

	getpeername(_fd, (struct sockaddr*) &addr, &len);
	inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
	_client_ip = ip;

	getsockname(_fd, (struct sockaddr*) &addr, &len);
	inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
	std::ostringstream oss;
	oss << ntohs(addr.sin_port);

	_host_ip_port = std::string(ip) + ":" + oss.str();
}

Client::Client(const Client &src) {
	*this = src;
}

Client& Client::operator=(const Client &src) {
	_method					= src._method;
	_request_target			= src._request_target;
	_headers				= src._headers;
	_body					= src._body;
	_query_string			= src._query_string;
	_path					= src._path;
	_status_code			= src._status_code;
	_process_request_line	= src._process_request_line;
	_process_headers		= src._process_headers;
	_process_body			= src._process_body;
	_request_is_complete	= src._request_is_complete;
	_timeout				= src._timeout;
	_query_string			= src._query_string;
	_path					= src._path;
	_fd						= src._fd;
	_virtual_servers		= src._virtual_servers;
	_conf					= src._conf;
	_path					= src._path;
	_client_ip				= src._client_ip;
	_host_ip_port			= src._host_ip_port;
	_request_line			= src._request_line;
	_syscall_error			= src._syscall_error;
	_if_location			= src._if_location;
	_location				= src._location;
	return *this;
}

Client::~Client() {}

std::string Client::get_method() const {
	return _method;
}

std::string Client::get_request_target() const {
	return _request_target;
}

std::map<std::string, std::string> Client::get_headers() const {
	return _headers;
}

int Client::get_fd() const {
	return _fd;
}

Config 	Client::get_conf() const{
	return _conf;
}

int Client::get_status_code() const {
	return _status_code;
}

std::string		Client::get_body() const{
	return _body;
}

std::string		Client::get_query_string() const{
	return _query_string;
}

std::string		Client::get_path() const {
	return _path;
}

bool	Client::get_request_is_complete() const {
	return _request_is_complete;
}
std::string	Client::get_client_ip() const {
	return _client_ip;
}

std::string	Client::get_host_ip_port() const {
	return _host_ip_port;
}
std::string Client::get_request_line() const {
	return _request_line;
}

bool Client::get_if_location() const {
	return _if_location;
}


void Client::log(std::string message, bool success)
{
	time_t rawtime;
	time(&rawtime);
	struct tm *timeinfo = localtime(&rawtime);

	if (success)
	{
		std::string date = asctime(timeinfo);
		date.erase(date.size() - 1);
		std::cout << BBLU <<  "[" << date << "] " << "[access] : " << message << RESET << std::endl;
	}
	else
	{
		std::string date = asctime(timeinfo);
		date.erase(date.size() - 1);
		std::cerr << BYEL <<  "[" << date << "] " << "[error] : " << message << RESET << std::endl;
	}
}

// access log format
// [access] : client: 127.0.0.1, request: "GET /favicon.ico HTTP/1.1", 404, host "localhost:8080"
std::string Client::log_access(int status_code) {
	std::string message;

	message += "client: ";
	message += get_client_ip() + ", ";
	message += "\"" + get_request_line() + "\", ";
	message += to_String(status_code) + ", ";
	message += "host: " + get_host_ip_port();

	return message;
}

// error log format
// [error]  : open() "/usr/share/nginx/html/my-site/another/favicon.ico" failed (2: No such file or directory), client: 127.0.0.1, request: "GET /favicon.ico HTTP/1.1", host: "localhost:8080"
std::string Client::log_error(std::string syscall_error) {
	std::string message;

	message += syscall_error + ", ";
	message += get_client_ip() + ", ";
	message += "\"" + get_request_line() + "\", ";
	message += "host: " + get_host_ip_port();
	return message;
}

// --------- ft_split ----------- //
int		ft_check_charset(char c, const char *charset)
{
	int i;

	i = 0;
	while (charset[i] != 0)
	{
		if (c == charset[i])
			return (1);
		i++;
	}
	return (0);
}

int		ft_count_words(const char *str, const char *charset)
{
	int len;

	len = 0;
	if (!(ft_check_charset(*str, charset)))
		len++;
	while (*str)
	{
		if (ft_check_charset(*str, charset))
		{
			while (ft_check_charset(*str, charset) && *str != 0)
				str++;
			len++;
		}
		else
			str++;
	}
	if (ft_check_charset(*(str - 1), charset))
		len--;
	return (len);
}

std::vector<std::string> Client::ft_split(const char *str, const char *charset)
{
	int		i;
	int		len;
	std::vector<std::string> tokens;

	while (*str != 0)
	{
		if (ft_check_charset(*str, charset))
			str++;
		else
		{
			len = 0;
			while (!(ft_check_charset(str[len], charset)) && str[len] != 0)
				len++;
			i = 0;
			std::string intermediate;
			while (i < len) {
				intermediate.push_back(*(str++));
				i++;
			}
			tokens.push_back(intermediate);
		}
	}
	return tokens;
}

void Client::start_timeout() {
	_timeout = std::time(NULL);
}

std::time_t Client::get_timeout() const {
	return std::time(NULL) - _timeout;
}
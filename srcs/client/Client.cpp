#include "Client.hpp"

Client::Client() {}

Client::Client(int fd, std::vector<Config> virtual_servers) :
	_code(0),
	_process_request_line(true),
	_process_headers(true),
	_process_body(true),
	_request_is_complete(false),
	_timeout(std::time(NULL)),
	_fd(fd),
	_virtual_servers(virtual_servers) {}

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
	_code					= src._code;
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

int Client::get_code() const {
	return _code;
}

std::string		Client::get_body() const{
	return _body;
}

std::string		Client::get_query_string() const{
	return _query_string;
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

Config Client::get_conf() const {
	return _conf;
}

void Client::clear() {
	*this = Client();
}
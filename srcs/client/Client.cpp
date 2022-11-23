#include "Client.hpp"

Client::Client() {}

Client::Client(int fd, Config conf) :
	_code(0),
	_process_request_line(true),
	_process_headers(true),
	_process_body(true),
	_timeout(std::time(NULL)),
	_fd(fd),
	_conf(conf) {}

Client::Client(const Client &src) {
	*this = src;
}

Client& Client::operator=(const Client &src) {
	_method					= src._method;
	_path					= src._path;
	_headers				= src._headers;
	_body					= src._body;
	_code					= src._code;
	_process_request_line	= src._process_request_line;
	_process_headers		= src._process_headers;
	_process_body			= src._process_body;
	_timeout				= src._timeout;
	_fd						= src._fd;
	_conf					= src._conf;
	return *this;
}

Client::~Client() {}

std::string Client::get_method() const {
	return _method;
}

std::string Client::get_path() const {
	return _path;
}

std::map<std::string, std::vector<std::string> > Client::get_headers() const {
	return _headers;
}

int Client::get_fd() const {
	return _fd;
}

int Client::get_code() const {
	return _code;
}
Config 	Client::get_conf() const{
	return _conf;
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

void Client::clear() {
	*this = Client();
}
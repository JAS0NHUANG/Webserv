#include "../../incs/Request.hpp"

Request::Request(int fd) : _fd(fd) {}

Request::Request(const Request &src) {
	*this = src;
}

Request& Request::operator=(const Request &src) {
	_method					= src._method;
	_path					= src._path;
	_headers				= src._headers;
	_requestLineHasBeenRead	= src._requestLineHasBeenRead;
	_hostLineHasBeenRead	= src._hostLineHasBeenRead;
	_isComplete				= src._isComplete;
	_timeout				= src._timeout;
	_fd						= src._fd;
	return *this;
}

Request::~Request() {}

void Request::remove_cr_char(std::deque<std::string> &lines) {
	std::deque<std::string>::iterator it;

	for (it = lines.begin(); it != lines.end(); it++) {
		if (!(*it).empty() && (*it)[(*it).size() - 1] == '\r')
			(*it).erase((*it).size() - 1, 1);
	}
}

void Request::parse_line(std::deque<std::string> &lines) {
	std::deque<std::string>::iterator it = lines.begin();

	remove_cr_char(lines);
	for (; it != lines.end(); it++)
		std::cout << "req_line : |" << *it << "|" << std::endl;
}

std::deque<std::string> Request::getlines(std::string buf) {
	std::deque<std::string> lines;
	_ss << buf;

	std::string::size_type i = buf.find("\n");
	while (i != std::string::npos) {
		std::string inter;
		std::getline(_ss, inter);
		lines.push_back(inter);
		buf.erase(0, i + 1);
		i = buf.find("\n");
	}
	return lines;
}

void Request::recv_buffer() {
	std::deque<std::string>	lines;
	char buffer[BUFFER_SIZE] = {0};

	while (true) {
		int valread = recv(_fd, buffer, BUFFER_SIZE - 1, 0);
		if (valread == 0) {
			return;
		}
		else if (valread < 0) {
			if (errno == EAGAIN)
				return;
			std::cerr << "recv\n" ; // NOTE : handle error
		}
		buffer[valread] = '\0';
		lines = getlines(buffer);
		if (!lines.empty())
			parse_line(lines);
	}
}

bool Request::is_complete() const {
	return _isComplete;
}

int Request::get_method() const {
	return _method;
}

std::string Request::get_path() const {
	return _path;
}

std::map<std::string, std::string> Request::get_headers() const {
	return _headers;
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

std::vector<std::string> Request::ft_split(const char *str, const char *charset)
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
			intermediate.push_back('\0');
			tokens.push_back(intermediate);
		}
	}
	return tokens;
}

void Request::debug() const {
	std::cout << "File descriptor: " << _fd << "\n";
}
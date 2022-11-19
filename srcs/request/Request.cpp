#include "../../incs/Request.hpp"

Request::Request(int fd) :
	_process_request_line(true),
	_host_header_received(false),
	_process_headers(true),
	_fd(fd) {}

Request::Request(const Request &src) {
	*this = src;
}

Request& Request::operator=(const Request &src) {
	_method					= src._method;
	_path					= src._path;
	_headers				= src._headers;
	_process_request_line	= src._process_request_line;
	_host_header_received	= src._host_header_received;
	_process_headers		= src._process_headers;
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

void Request::process_request_line(std::vector<std::string> &tokens) {
	std::cout << "process_request_line\n";
	std::vector<std::string>::iterator it = tokens.begin();
	for (; it != tokens.end() && (*it).empty(); it++) ;

	if (it == tokens.end())
		return;

	// PARSE (METHOD / PATH / VERSION)
	if (*it == "GET")
		_method = GET;
	else if (*it == "POST")
		_method = POST;
	else if (*it == "DELETE")
		_method = DELETE;
	else {
		std::cerr << RED "400 BAD REQUEST BAD METHOD\n" RESET;
		return;
	}

	if (++it == tokens.end()) {
		std::cerr << RED "400 BAD REQUEST MISSING PATH\n" RESET;
		return;
	}

	_path = *it;

	if (++it == tokens.end()) {
		std::cerr << RED "400 BAD REQUEST MISSING VERSION" RESET;
		return;
	}

	if (*it != "HTTP/1.1") {
		std::cerr << RED "400 BAD REQUEST BAD VERSION\n" RESET;
		return;
	}

	_process_request_line = false;
}

// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Host

// https://developer.mozilla.org/en-US/docs/Glossary/Forbidden_header_name
// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers

// NOTE : What to do if same header names are received ???
// NOTE
void Request::process_header(std::vector<std::string> &tokens) {
	std::cout << "process_headers\n";
	std::vector<std::string>::iterator it = tokens.begin();

	if (it == tokens.end()) {
		std::cout << "NO MORE HEADERS TO READ.\n";
		if (!_host_header_received)
			std::cerr << RED "400 BAD REQUEST HOST HEADER NOT RECEIVED\n" RESET;
		return;
	}

	if ((*it)[(*it).size() - 1] != ':') {
		std::cerr << RED "400 BAD REQUEST HEADER NAME DO NOT CONTAIN ':'\n" RESET;
		std::cout << "Header was |" << *it << "|\n"; 
		return;
	}
	(*it).erase((*it).size() - 1, 1); // Erasing ':'

	// NOTE : To continue ...

	// std::string name(*it);
	// ++it;
	// if (it == tokens.end())
	// _headers[*it] 
}

void Request::process_body(std::vector<std::string> &tokens) {
	(void)tokens;
	std::cout << "process_body\n";
}

// NOTE : A verifier qu'il y a bien dans le deque une entree vide 
// a l'endroit ou il y aurait potentiellement une ligne vide 
// dans la requete
void Request::parse_line(std::deque<std::string> &lines) {
	std::deque<std::string>::iterator it = lines.begin();

	remove_cr_char(lines);

	for (; it != lines.end(); it++)
		std::cout << "req_line : |" << *it << "|" << std::endl;

	std::vector<std::string> tokens;
	while (!lines.empty()) {
		tokens = ft_split(lines.front().c_str(), "\t\v\r ");

		for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); it++)
			std::cout << "TOKEN : |" << *it << "|\n";

		if (_process_request_line)
			process_request_line(tokens);
		else if (_process_headers)
			process_header(tokens);
		else
			process_body(tokens);

		lines.pop_front();
	}
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
			tokens.push_back(intermediate);
		}
	}
	return tokens;
}

void Request::debug() const {
	std::cout << "File descriptor: " << _fd << "\n";
}
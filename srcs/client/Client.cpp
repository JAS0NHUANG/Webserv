#include "../../incs/Client.hpp"

Client::Client() {}

Client::Client(int fd) :
	_process_request_line(true),
	_host_header_received(false),
	_process_headers(true),
	_fd(fd) {}

Client::Client(const Client &src) {
	*this = src;
}

Client& Client::operator=(const Client &src) {
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

Client::~Client() {}

void Client::remove_cr_char(std::deque<std::string> &lines) {
	std::deque<std::string>::iterator it;

	for (it = lines.begin(); it != lines.end(); it++) {
		if (!(*it).empty() && (*it)[(*it).size() - 1] == '\r') 
			(*it).erase((*it).size() - 1, 1);
	}
}

void Client::process_request_line(std::deque<std::string> &lines) {
	std::cout << "process_request_line\n";

	std::vector<std::string> tokens = ft_split(lines.front().c_str(), "\t\v\r ");
	std::vector<std::string>::iterator it = tokens.begin();
	for (; it != tokens.end() && (*it).empty(); it++) ;

	if (it == tokens.end())
		return;

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
void Client::process_header(std::deque<std::string> &lines) {
	std::cout << "process_headers\n";
	std::vector<std::string> tokens = ft_split(lines.front().c_str(), "\t\v\r ");
	std::vector<std::string>::iterator it = tokens.begin();

	if (it == tokens.end()) {
		std::cout << "NO MORE HEADERS TO READ.\n";
		if (!_host_header_received)
			std::cerr << RED "400 BAD REQUEST HOST HEADER NOT RECEIVED\n" RESET;
		else
			_process_headers = false;
		return;
	}

	if ((*it)[(*it).size() - 1] != ':') {
		std::cerr << RED "400 BAD REQUEST HEADER NAME DO NOT CONTAIN ':'\n" RESET;
		std::cout << "Header was |" << *it << "|\n"; 
		return;
	}

	std::string header_name(*it);
	if (header_name == "Host:")
		_host_header_received = true;

	++it;
	if (it == tokens.end()) {
		// NOTE : is it really an error
		std::cerr << RED "400 BAD REQUEST HEADER FIELD DO CONTAINS VALUE\n" RESET; 
		return;
	}
	_headers[header_name] = *it;
	if (++it != tokens.end()) {
		std::cerr << RED "Care there is more header value tokens that will go in the void\n" RESET;
	}
}

void Client::process_body(std::deque<std::string> &lines) {
	// NOTE : The body must not be tokenize
	// That means ft_split is at the wrong place

	(void)lines;
	std::cout << "process_body\n";
}

// NOTE : A verifier qu'il y a bien dans le deque une entree vide 
// a l'endroit ou il y aurait potentiellement une ligne vide 
// dans la requete
void Client::parse_line(std::deque<std::string> &lines) {
	std::deque<std::string>::iterator it = lines.begin();

	remove_cr_char(lines);

	for (; it != lines.end(); it++)
		std::cout << "req_line : |" << *it << "|" << std::endl;


	while (!lines.empty()) {
		if (_process_request_line)
			process_request_line(lines);
		else if (_process_headers) {
			process_header(lines);
			// Maybe parse the header right here to know what to do next
		}
		else
			process_body(lines);

		lines.pop_front();
	}
}

std::deque<std::string> Client::getlines(std::string buf) {
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

void Client::recv_request() {
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

void Client::send_response() {
	// NOTE : to dev
	std::string response("HTTP/1.1 200 OK\n\nHello world\n");
	if (send(_fd, response.c_str(), response.size(), 0) < 0)
		errMsgErrno("send failed");
}

bool Client::is_complete() const {
	return _isComplete;
}

int Client::get_method() const {
	return _method;
}

std::string Client::get_path() const {
	return _path;
}

std::map<std::string, std::string> Client::get_headers() const {
	return _headers;
}

int Client::get_fd() const {
	return _fd;
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

void Client::debug() const {
	std::cout << "File descriptor: " << _fd << "\n";
}
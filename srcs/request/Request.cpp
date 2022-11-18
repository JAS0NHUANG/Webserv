#include "../../incs/Request.hpp"

Request::Request() {}

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

int BUFFER_SIZE = 2056;

void Request::parse_line(std::string &line) {
	(void)line;
}

std::pair<bool, std::string> Request::check_if_buffer_has_nl(std::string buf) {
	std::string line;
	
	_ss << buf;
	std::string::size_type i = buf.find("\n");
	if (i != std::string::npos) {
		std::getline(_ss, line);
		return std::pair<bool, std::string>(true, line);
	}
	return std::pair<bool, std::string>(false, "");

}

void Request::recv_buffer() {
	std::pair<bool, std::string> pr;
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
		pr = check_if_buffer_has_nl(buffer);
		if (pr.first == true) {
			parse_line(pr.second);
			std::cout << "Request(class) line : " << pr.second << "\n";
		}
	}
	
}

bool Request::is_complete() const {
	return _isComplete;
}
void Request::debug() const {
	std::cout << "File descriptor: " << _fd << "\n";
}
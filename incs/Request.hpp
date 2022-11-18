#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include <map>

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

class Request {
	protected:
		int									_method;
		std::string							_path;
		std::map<std::string, std::string>	_headers;
		std::stringstream					_ss;
		bool								_requestLineHasBeenRead;
		bool								_hostLineHasBeenRead;
		bool								_isComplete;
		int									_timeout;
		int									_fd;
		std::pair<bool, std::string>		check_if_buffer_has_nl(std::string buf);
		void								parse_line(std::string &line);
		Request(); // file descriptor is mandatory to instantiate this object
	public:
		Request(int fd);
		Request(const Request &src);
		Request& operator=(const Request &src);
		~Request();

		void recv_buffer();
		bool is_complete() const;

		void debug() const;
};

#endif
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "webserv.hpp"

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
		bool								_process_request_line;
		bool								_host_header_received;
		bool								_process_headers;
		bool								_isComplete;
		int									_timeout;
		int									_fd;

		std::deque<std::string>				getlines(std::string buf);
		void								parse_line(std::deque<std::string> &lines);
		void								process_request_line(std::deque<std::string> &lines);
		void								process_header(std::deque<std::string> &lines);
		void								process_body(std::deque<std::string> &lines);
		std::vector<std::string>			ft_split(const char *str, const char *charset);
		void								remove_cr_char(std::deque<std::string> &lines);
		Request();

	public:
		Request(int fd);
		Request(const Request &src);
		Request& operator=(const Request &src);
		~Request();

		void recv_buffer();
		bool is_complete() const;

		// getters
		int									get_method() const;
		std::string							get_path() const;
		std::map<std::string, std::string>	get_headers() const;

		void debug() const;
};

#endif
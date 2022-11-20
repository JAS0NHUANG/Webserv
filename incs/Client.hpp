#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "webserv.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include <map>
#include <ctime>

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

class Client {
	protected:
		// underlying datas
		int									_method;
		std::string							_path;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		int									_code;
		std::stringstream					_ss;
		bool								_process_request_line;
		bool								_host_header_received;
		bool								_process_headers;
		std::time_t							_timeout;
		int									_fd;
		Server								_conf;

		// parsing
		void								parse_line(std::deque<std::string> &lines);
		void								process_request_line(std::string &line);
		void								process_header(std::string &line);
		void								process_body(std::string &line);

		// parsing utils
		void								remove_cr_char(std::deque<std::string> &lines);
		std::deque<std::string>				getlines(std::string buf);
		std::vector<std::string>			ft_split(const char *str, const char *charset);

		// timeout
		void								start_timeout();
		std::time_t							get_timeout() const;

		void								send_client_error_response() const;

	public:
		Client();
		Client(int fd, Server conf);
		Client(const Client &src);
		Client& operator=(const Client &src);
		~Client();

		// received from client / send to client
		bool recv_request();
		bool send_response();

		// getters
		int									get_method() const;
		std::string							get_path() const;
		std::map<std::string, std::string>	get_headers() const;
		int									get_fd() const;

		void debug() const;
};

#endif
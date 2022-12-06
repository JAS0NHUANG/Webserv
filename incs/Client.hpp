#ifndef CLIENT_HPP
 #define CLIENT_HPP

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
		std::string							_method;
		std::string							_request_target;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		std::string							_query_string;
		std::string							_path;
		int									_code;
		std::stringstream					_ss;
		bool								_process_request_line;
		bool								_process_headers;
		bool								_process_body;
		bool								_request_is_complete;
		std::time_t							_timeout;
		int									_fd;
		std::vector<Config>					_virtual_servers;
		Config								_conf;

		// parsing
		void								parse_line(std::deque<std::string> &lines);
		void								process_request_line(std::string &line);
		std::string							get_query_string(std::string &request_target);
		std::string							get_path(std::string request_target);
		void								check_access(std::string request_target);
		void							check_method(std::string &method);
		void								process_field_line(std::string &line);
		bool								field_name_has_whitespace(std::string &field_name) const;
		void								retrieve_conf(std::string host);
		void								process_body(std::string &line);

		// parsing utils
		void								remove_cr_char(std::deque<std::string> &lines);
		std::deque<std::string>				getlines(std::string buf);
		std::vector<std::string>			ft_split(const char *str, const char *charset);

		// timeout
		void								start_timeout();
		std::time_t							get_timeout() const;

		bool								send_client_error_response() const;
		bool								send_server_error_response() const;
		bool								send_successful_response() const;
		bool								send_redirection_message() const;
		bool								send_informational_response() const;

		void								clear();

	public:
		Client();
		Client(int fd, std::vector<Config> virtual_servers);
		Client(const Client &src);
		Client& operator=(const Client &src);
		~Client();

		// received from client / send to client
		bool recv_request();
		bool send_response();

		// getters
		std::string							get_method() const;
		std::string							get_request_target() const;
		std::string							get_query_string() const;
		std::map<std::string, std::string>	get_headers() const;
		int									get_fd() const;
		Config 								get_conf() const;
		int 								get_code() const;
		std::string							get_body() const;
		std::string							get_path2() const {
			return _path;
		}
};

#endif
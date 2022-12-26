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
		std::string							_syscall_error;
		int									_status_code;
		std::stringstream					_ss;
		bool								_process_request_line;
		bool								_process_headers;
		bool								_process_body;
		bool								_request_is_complete;
		std::time_t							_timeout;
		int									_fd;
		std::string							_client_ip;
		std::string							_host_ip_port;
		std::string							_request_line;
		std::vector<Config>					_virtual_servers;
		bool								_if_location;
		std::string							_server_name;
		Config								_conf;
		Location							_location;
		// add body boundary
		std::string							_raw_request;
		std::string							_body_boundary;
		bool								_is_upload_ok;
		// parsing
		void								parse_line(std::deque<std::string> &lines, std::string &raw_request);
		void								process_request_line(std::string &line);
		std::string							get_query_string(std::string &request_target);
		void								check_access(std::string request_target);
		bool								check_if_location_required(std::string &request_target);
		void								check_method(std::string &method);
		void								process_field_line(std::string &line);
		bool								field_name_has_whitespace(std::string &field_name) const;
		void								retrieve_conf(std::string host);
		void								process_body(std::string &raw_request);
		bool								upload_file(std::string &raw_request);
		bool								check_boundary(std::string &raw_request);

		// parsing utils
		void								remove_cr_char(std::deque<std::string> &lines);
		std::deque<std::string>				getlines(std::string buf);
		std::vector<std::string>			ft_split(const char *str, const char *charset);

	public:
		std::string							create_path(std::string request_target);
		Client();
		Client(int fd, std::vector<Config> virtual_servers);
		Client(const Client &src);
		Client& operator=(const Client &src);
		~Client();

		// received from client / send to client
		void								recv_request();
		bool								handle_request();

		void								log(std::string message, bool success);
		std::string							log_access(int status_code);
		std::string							log_error(std::string syscall_error);

		// timeout
		void								start_timeout();
		std::time_t							get_timeout() const;
		void								set_timeout_status_code();

		// getters
		std::string							get_method() const;
		std::string							get_request_target() const;
		std::string							get_query_string() const;
		std::map<std::string, std::string>	get_headers() const;
		int									get_fd() const;
		Config 								get_conf() const;
		int 								get_status_code() const;
		std::string							get_body() const;
		std::string							get_path() const;
		bool								get_request_is_complete() const;
		std::string							get_client_ip() const;
		std::string							get_host_ip_port() const;
		std::string							get_request_line() const;
		bool								get_if_location() const;
		std::string							get_server_name() const;
};

#endif

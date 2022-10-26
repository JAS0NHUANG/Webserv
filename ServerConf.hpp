#ifndef SERVER_CONF_HPP
#define SERVER_CONF_HPP

#include "webserv.hpp"

class ServerConf : public BaseConf {
	private:
		/* Just the address that webserv will listen for */
		std::string	_address;

		/* The port number */
		int	_port;

		/* A vector of server names */
		std::vector<std::string>	_server_name;

		/* [ error_code | path_to_error_page ] */
		std::map<int, std::string>	_error_page;

		/* Size of the maximum bytes a client can send to webserv */
		unsigned long long	_client_max_body_size;

		/* [ uri | uri_configuration ] */
		std::map<std::string, BaseConf>	_location;
	public:

		void debug() const;

		ServerConf();
		~ServerConf();
		ServerConf(const ServerConf &x);
		ServerConf& operator=(const ServerConf &x);

		std::string					get_address() const; // Default is *
		int							get_port() const; // Default is 80
		std::vector<std::string>	get_server_name() const; // Return empty vector if not set
		std::string					get_error_page(int code) const; // Return empty string if not set
		unsigned long long			get_client_max_body_size() const; // Default is 1M
		std::pair<bool, BaseConf>	get_location(std::string uri) const; // If uri is not found, bool is false

		void	set_address(std::string str);
		void	set_port(int port);
		void	set_server_name(std::vector<std::string> names);
		void	set_error_page(std::vector<int> codes, std::string path);
		void	set_client_max_body_size(unsigned long long size);
		void	set_location(std::string uri, BaseConf location);


		class	NotFoundException : public std::exception {
			public:
				char const* what(void) const throw();
		};
};

void debug_print(std::map<std::string, BaseConf> x);


#endif
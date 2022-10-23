#ifndef SERVER_CONF
#define SERVER_CONF

#include "BaseConf.hpp"

class ServerConf : public BaseConf {
	private:
		std::string							_address;				// NOTE: what is default
		int									_port;					// Default 80
		std::vector<std::string>			_server_name; 			// Default = ""
		std::map<int, std::string>			_error_page; 			// Size = 0 if not set
		unsigned long long					_client_max_body_size;	// Default 1M
		std::map<std::string, BaseConf>		_location;				// Size = 0 if not set.
	public:

		void debug() const;

		ServerConf(); // NOTE: Check if BaseConf is well construct
		~ServerConf();
		ServerConf(const ServerConf &x);
		ServerConf& operator=(const ServerConf &x);

		std::string					get_address() const;
		int							get_port() const;
		std::vector<std::string>	get_server_name() const; // throw an exeption if not set
		std::string					get_error_page(int code) const; // throw an exeption if not set
		unsigned long long			get_client_max_body_size() const; // Default 1M

		class	NotFoundException : public std::exception {
			public:
				char const* what(void) const throw();
		};
};

void debug_print(std::map<std::string, BaseConf> x);


#endif
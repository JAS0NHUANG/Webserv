#ifndef BASE_CONF_HPP
#define BASE_CONF_HPP

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include "colorcodes.hpp"

#define GET		1
#define POST	2
#define DELETE	3

class BaseConf {
	protected:
		std::set<int>				_allow_method;	// methods are defined above, the default is GET, POST, DELETE
		std::map<int, std::string>	_return;		// int for code, string for holding URL or text; ex: ["IS_URL"/"IS_TEXT", URL/text]
		std::string					_root;			// "path"
		bool						_autoindex;		// true/false; default = false
		std::vector<std::string>	_index;			// [text, ...] ; default = index, index.html
		std::vector<std::string>	_cgi;			// ["IS_SET", extension, path];
		bool						_upload_store;	// true/false; default = false

	public:

		void	debug() const;

		BaseConf();
		BaseConf(const BaseConf &x);
		~BaseConf();
		BaseConf& operator=(const BaseConf &x);

		std::string					get_root() const;
		std::vector<std::string>	get_index() const;

		bool	is_method_allowed(int method) const;
		bool	is_autoindex_on() const;
		bool	is_cgi_set() const;
		bool	is_upload_on() const;
		bool	is_return_set() const;
};

void debug_print(std::set<int> x);
void debug_print(std::map<int, std::string> x);
void debug_print(std::vector<std::string> x);

#endif
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

#define IS_URL	1
#define IS_TEXT	2

class Location {
	protected:
		/* A set of defined methods */
		std::set<int>	_allow_method;

		/* [ return code | (url or text | string) ] */
		std::map<int, std::pair<int, std::string> >	_return;

		/* Just the path where webserv will considered as its root to search files, etc... */
		std::string	_root;

		/* true or false to know if autoindex is set or not */
		bool	_autoindex;

		/* vector that hold default page name */
		std::vector<std::string>	_index;

		/* [ is_set/is_not_set | (extension | path_to_cgi_bin) ] */
		std::pair<bool, std::pair<std::string, std::string> >	_cgi;

		/* [ is_on/is_off | directory ] */
		std::pair<bool, std::string>	_upload_store;

	public:

		void	debug() const;

		Location();
		Location(const Location &x);
		~Location();
		Location& operator=(const Location &x);

		std::string					get_root() const;
		std::vector<std::string>	get_index() const;

		bool	is_method_allowed(int method) const;
		bool	is_autoindex_on() const;
		bool	is_cgi_set() const;
		bool	is_upload_on() const;
		bool	is_return_set() const;

		void	set_allow_method(std::vector<int> &vec);
		void	set_return(int &code, std::string &str, int &type);
		void	set_root(std::string &str);
		void	set_autoindex(bool b);
		void	set_index(std::vector<std::string> &vec);
		void	set_cgi(std::string &ext, std::string &path);
		void	set_upload_store(std::string &dir);
};

void debug_print(std::set<int> x);
void debug_print(std::map<int, std::string> x);
void debug_print(std::vector<std::string> x);
void debug_print(std::map<int, std::pair<int, std::string> > x);
void debug_print(std::pair<bool, std::pair<std::string, std::string> > x);
void debug_print(std::pair<bool, std::string> x);

#endif
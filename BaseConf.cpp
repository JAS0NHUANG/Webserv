#include "BaseConf.hpp"

void debug_print(std::set<int> x) {
	for (std::set<int>::iterator it = x.begin(); it != x.end(); it++)
		std::cout << *it << " ";
	std::cout << "\n";
}

void debug_print(std::map<int, std::string> x) {
	for (std::map<int, std::string>::iterator it = x.begin(); it != x.end(); it++)
		std::cout << it->second << " ";
	std::cout << "\n";
}

void debug_print(std::vector<std::string> x) {
	for (std::vector<std::string>::iterator it = x.begin(); it != x.end(); it++)
		std::cout << *it << " ";
	std::cout << "\n";
}
void debug_print(std::map<int, std::pair<int, std::string> > x) {
	std::map<int, std::pair<int, std::string> >::iterator it = x.begin();
	for (; it != x.end(); it++)
		std::cout << "[ " << it->first << " | " << it->second.first << " | " << it->second.second << "] ";
	std::cout << "\n";
}

void debug_print(std::pair<bool, std::pair<std::string, std::string> > x) {
	std::cout << "[ " << x.first << " | ( " << x.second.first << " | " << x.second.second << " ) ]\n";
}

void debug_print(std::pair<bool, std::string> x) {
	std::cout << "[ " << x.first << " | " << x.second << " ]\n";
}

void	BaseConf::debug() const {
	std::cout << MAG ;
	std::cout << "Debugging BaseConf\n";
	std::cout << "allow_method : "; debug_print(_allow_method);
	std::cout << "return       : "; debug_print(_return);
	std::cout << "root         : " << _root	<< "\n";
	std::cout << "autoindex    : " << _autoindex << "\n";
	std::cout << "index        : "; debug_print(_index);
	std::cout << "cgi          : "; debug_print(_cgi);
	std::cout << "upload_store : "; debug_print(_upload_store);
	std::cout << RESET ;
}


BaseConf::BaseConf() {
	_allow_method.insert(POST);
	_allow_method.insert(GET);
	_allow_method.insert(DELETE);

	_root = "/var/www/html";
	_autoindex = false;
	_index.push_back("index");
	_index.push_back("index.html");
	_cgi.first = false;
	_upload_store.first = false;
}

BaseConf::BaseConf(const BaseConf &x) {
	*this = x;
}

BaseConf::~BaseConf() {}

BaseConf& BaseConf::operator=(const BaseConf &x) {
	_allow_method	= x._allow_method;
	_return			= x._return;
	_root			= x._root;
	_autoindex		= x._autoindex;
	_index			= x._index;
	_cgi			= x._cgi;
	_upload_store	= x._upload_store;
	return *this;
}

std::vector<std::string> BaseConf::get_index() const {
	return _index;
}

bool BaseConf::is_method_allowed(int method) const {
	if (_allow_method.count(method))
		return true;
	return false;
}

bool BaseConf::is_autoindex_on() const {
	return _autoindex;
}

bool BaseConf::is_cgi_set() const {
	return _cgi.first == true;
}

bool BaseConf::is_upload_on() const {
	return _upload_store.first;
}

bool BaseConf::is_return_set() const {
	return _return.size() != 0;
}

void BaseConf::set_allow_method(std::vector<int> vec) {
	_allow_method.clear();
	_allow_method.insert(vec.begin(), vec.end());
}

void	BaseConf::set_return(int code, std::string str, int type) {
	_return.erase(code);
	_return.insert(std::make_pair(code, std::make_pair(type, str)));
}

void	BaseConf::set_root(std::string str) {
	_root = str;
}

void	BaseConf::set_autoindex(bool b) {
	_autoindex = b;
}

void	BaseConf::set_index(std::vector<std::string> vec) {
	_index.clear();
	_index.assign(vec.begin(), vec.end());
}

void	BaseConf::set_cgi(std::string ext, std::string path) {
	_cgi.first = true;
	_cgi.second = std::make_pair(ext, path);
}

void	BaseConf::set_upload_store(std::string dir) {
	_upload_store.first = true;
	_upload_store.second = dir;
}

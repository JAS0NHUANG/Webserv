#ifndef CONFIG_PARSING_HPP
#define CONFIG_PARSING_HPP

enum directives {
	e_unknown,
	e_server,
	e_neutral,
	e_listen,
	e_server_name,
	e_allow_method,
	e_cgi,
	e_error_page,
	e_client_max_body_size,
	e_location,
	e_return,
	e_root,
	e_autoindex,
	e_index,
	e_upload_store
};

// saveFile.cpp
std::vector<std::string> split_line(std::string str);
void delete_comment(std::vector<std::string> &vec);
void save_file(char *fileName, std::queue<std::vector<std::string> >	&qu);

// configParsing.cpp
void call_doers(std::queue<std::vector<std::string> > &qu, Location &conf, int &line);
void config_parse(std::queue<std::vector<std::string> > &qu, std::vector<Config> &conf);
void debugPrintQ(std::queue<std::vector<std::string> >	&qu);
void parse_file(char *fileName, std::map<std::string, std::vector<Config> > &virtual_servers);

// doParsing.cpp
void do_server_parsing(std::queue<std::vector<std::string> > &qu, int &line);
void do_listen_parsing(std::queue<std::vector<std::string> > &qu, Config &conf, int &line);
void do_server_name_parsing(std::queue<std::vector<std::string> > &qu, Config &conf, int &line);
void do_error_page_parsing(std::queue<std::vector<std::string> > &qu, Config &conf, int &line);
void do_client_max_body_size_parsing(std::queue<std::vector<std::string> > &qu, Config &conf, int &line);
void do_location_parsing(std::queue<std::vector<std::string> > &qu, Config &conf, int &line);
void do_allow_method_parsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line);
void do_return_parsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line);
void do_root_parsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line);
void do_autoindex_parsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line);
void do_index_parsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line);
void do_cgi_parsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line);
void do_upload_store_parsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line);

// utilsParsing.cpp
void err_msg_errno(std::string msg);
void erase_token(std::queue<std::vector<std::string> > &qu, int &line);
void check_if_line_is_empty(std::queue<std::vector<std::string> > &qu, int &line);
directives find_directive(std::string &str);
std::string to_String(int n);
int to_int(std::string str);
unsigned long long to_ULL(std::string str);
bool is_valid_ip_address(std::string str);
bool is_valid_port(std::string str);
bool is_http_error_status_code(std::string str);
bool is_redirect_status_code(std::string str);
void str_to_upper(std::string &str);
bool is_whitespace(unsigned char c);
bool is_num(std::string str);
bool is_valid_method(std::string str);
std::string::iterator skip_whitespace(std::string &str, std::string::iterator it);
bool is_server_directive(std::string &str);
bool is_directive(std::string str);
void throw_if_file_is_empty(std::queue<std::vector<std::string> > &qu, int line);
void throw_parsing_error( std::string msg, std::string line, std::string bName = "", std::string fName = "");

#endif

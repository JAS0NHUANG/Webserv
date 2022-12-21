#include "webserv.hpp"

void call_doers(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	directives d = find_directive(qu.front().front());

	if (d == e_allow_method)
		do_allow_method_parsing(qu, conf, line);
	else if (d == e_cgi)
		do_cgi_parsing(qu, conf, line);
	else if (d == e_return)
		do_return_parsing(qu, conf, line);
	else if (d == e_root)
		do_root_parsing(qu, conf, line);
	else if (d == e_autoindex)
		do_autoindex_parsing(qu, conf, line);
	else if (d == e_index)
		do_index_parsing(qu, conf, line);
	else if (d == e_upload_store)
		do_upload_store_parsing(qu, conf, line);
	else
		throw_parsing_error(std::string("unexpected '" + qu.front().front() + "'"), to_String(line));
}

void config_parse(std::queue<std::vector<std::string> > &qu, std::vector<Config> &conf) {

	int line		= 1;
	directives d	= e_neutral;
	Config c;

	if (qu.empty())
		std::cerr << "File is empty. Please give a minimal configuration.\n";

	while (qu.size()) {

		// Skip empty lines
		while (!qu.empty() && qu.front().empty()) {
			qu.pop();
			++line;
		}

		if (d == e_neutral) {
			do_server_parsing(qu, line);
			c = Config();
			// conf.push_back(Config());
			d = e_server;
		}

		if (d == e_server) {
			if (qu.empty())
				throw_parsing_error("expected '}'", to_String(line));

			if (qu.front().front() == "}") {
				erase_token(qu, line);
				d = e_neutral;
				conf.push_back(c);
				continue;
			}

			d = find_directive(qu.front().front());
			if (d == e_unknown)
				throw_parsing_error(std::string("unexpected '" + qu.front().front() + "'"), to_String(line));

			if (d == e_listen)
				do_listen_parsing(qu, c, line);
			else if (d == e_server_name)
				do_server_name_parsing(qu, c, line);
			else if (d == e_error_page)
				do_error_page_parsing(qu, c, line);
			else if (d == e_client_max_body_size)
				do_client_max_body_size_parsing(qu, c, line);
			else if (d == e_set_cookie)
				do_set_cookie_parsing(qu, c, line);
			else if (d == e_location)
				do_location_parsing(qu, c, line);
			else
				call_doers(qu, c, line);

			d = e_server;
		}
	}

	if (d == e_server)
		throw_parsing_error("expected '}' to close 'server' directive", to_String(line));
}

void debugPrintQ(std::queue<std::vector<std::string> >	&qu) {
	int line = 1;
	while (qu.size()) {
		std::vector<std::string>::iterator it = qu.front().begin();
		int i = 0;
		for (; it != qu.front().end(); it++) {
				std::cout << *it << "|";
				i++;
		}
		std::cout << "~~~vec size: " << i << " --line:" << line << "\n";
		qu.pop();
		++line;
	}
}

void parse_file(char *fileName, std::map<std::string, std::vector<Config> > &virtual_servers)
{
	std::vector<Config> conf;
	std::queue<std::vector<std::string> >	qu;

	// Save the file into a queue
	save_file(fileName, qu);

	//If debug is used, qu will be empty
	// debugPrintQ(qu);

	// Parse the file
	config_parse(qu, conf);

	// Save the virtual server that have same address:port 
	// in a unique entry in a map;
	std::vector<Config>::iterator it = conf.begin();
	for (; it != conf.end(); it++) {
		std::string key = (*it).get_address() + to_String((*it).get_port());
		virtual_servers[key].push_back(*it);
	}
}

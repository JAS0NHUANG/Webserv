#include "webserv.hpp"

void callDoers(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	directives d = findDirective(qu.front().front());

	if (d == e_allow_method)
		doAllowMethodParsing(qu, conf, line);
	else if (d == e_cgi)
		doCgiParsing(qu, conf, line);
	else if (d == e_return)
		doReturnParsing(qu, conf, line);
	else if (d == e_root)
		doRootParsing(qu, conf, line);
	else if (d == e_autoindex)
		doAutoindexParsing(qu, conf, line);
	else if (d == e_index)
		doIndexParsing(qu, conf, line);
	else if (d == e_upload_store)
		doUploadStoreParsing(qu, conf, line);
	else
		throwParsingError(std::string("unexpected '" + qu.front().front() + "'"), toString(line));
}

void configParse(std::queue<std::vector<std::string> > &qu, std::vector<Config> &conf) {

	int line		= 1;
	directives d	= e_neutral;

	if (qu.empty())
		std::cerr << "File is empty. Please give a minimal configuration.\n";

	while (qu.size()) {

		// Skip empty lines
		while (!qu.empty() && qu.front().empty()) {
			qu.pop();
			++line;
		}

		if (d == e_neutral) {
			doServerParsing(qu, line);
			conf.push_back(Config());
			d = e_server;
		}
		
		if (d == e_server) {
			if (qu.empty())
				throwParsingError("expected '}'", toString(line));

			if (qu.front().front() == "}") {
				eraseToken(qu, line);
				d = e_neutral;
				continue;
			}

			d = findDirective(qu.front().front());
			if (d == e_unknown)
				throwParsingError(std::string("unexpected '" + qu.front().front() + "'"), toString(line));
			
			if (d == e_listen)
				doListenParsing(qu, conf.front(), line);
			else if (d == e_server_name)
				doServerNameParsing(qu, conf.front(), line);
			else if (d == e_error_page)
				doErrorPageParsing(qu, conf.front(), line);
			else if (d == e_client_max_body_size)
				doClientMaxBodySizeParsing(qu, conf.front(), line);
			else if (d == e_location)
				doLocationParsing(qu, conf.front(), line);
			else
				callDoers(qu, conf.front(), line);
			
			d = e_server;

		}
	}

	if (d == e_server)
		throwParsingError("expected '}' to close 'server' directive", toString(line));
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

void parseFile(char *fileName, std::vector<Config> &conf)
{
	std::queue<std::vector<std::string> >	qu;

	saveFile(fileName, qu);

	//If debug is used, qu will be empty
	// debugPrintQ(qu);

	configParse(qu, conf);
}

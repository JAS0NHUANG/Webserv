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
		throwParsingError(qu.front().front(), toString(line), UNEXPECTED);
}

void configParse(std::queue<std::vector<std::string> > &qu, std::vector<Server> &conf) {

	int line		= 1;
	directives d	= e_neutral;

	if (isFileEmpty(qu))
		std::cerr << "File is empty. Please give a minimal configuration.\n";

	while (qu.size()) {
		if (d == e_neutral) {
			doServerParsing(qu, line);
			conf.push_back(Server());
			d = e_server;
		}
		
		if (d == e_server) {
			if (isFileEmpty(qu))
				throwParsingError("}", toString(line), EXPECTED);

			if (isCloseBracket(qu.front().front())) {
				eraseToken(qu, line);
				d = e_neutral;
				continue;
			}

			d = findDirective(qu.front().front());
			if (d == e_unknown)
				throwParsingError(qu.front().front(), toString(line), UNEXPECTED);
			
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
		std::cerr << "Expected '}' to close 'server' directive\n";
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

void parseFile(char *fileName, std::vector<Server> &conf)
{
	std::queue<std::vector<std::string> >	qu;

	saveFile(fileName, qu);

	//If debug is used, qu will be empty
	// debugPrintQ(qu);

	configParse(qu, conf);
}
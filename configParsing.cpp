#include "webserv.hpp"

void callDoers(std::queue<std::vector<std::string> > &qu, ServerConf &conf, int &line) {
	directives d = findDirective(qu.front()[0]);

	if (d == e_unknown)
		throwParsingError(qu.front()[0], toString(line), UNEXPECTED);

	if (d == e_allow_method)
		doAllowMethodParsing(qu, conf, line);
	else if (d == e_cgi)
		doCgiParsing(qu, conf, line);
	else if (d == e_error_page)
		doErrorPageParsing(qu, conf, line);
	else if (d == e_client_max_body_size)
		doClientMaxBodySizeParsing(qu, conf, line);
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
}

void configParse(std::queue<std::vector<std::string> > &qu, std::vector<ServerConf> &conf) {

	int line		= 1;
	directives d	= e_neutral;

	if (isFileEmpty(qu))
		std::cerr << "File is empty. Please give a minimal configuration.\n";

	while (qu.size()) {
		if (d == e_neutral) {
			doServerParsing(qu, line);
			// NOTE : push_back ServerConf in the vector
			d = e_server;
		}
		
		if (d == e_server) {
			if (isFileEmpty(qu))
				throwParsingError("}", toString(line), EXPECTED);

			if (isCloseBracket(qu.front()[0])) {
				eraseToken(qu, line);
				d = e_neutral;
				continue;
			}

			d = findDirective(qu.front()[0]);
			if (d == e_unknown)
				throwParsingError(qu.front()[0], toString(line), UNEXPECTED);
			
			if (d == e_listen)
				doListenParsing(qu, conf.front(), line);
			else if (d == e_server_name)
				doServerNameParsing(qu, conf.front(), line);
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
	while (qu.size()) {
	std::vector<std::string>::iterator it = qu.front().begin();
	int i = 0;
	for (; it != qu.front().end(); it++) {
			std::cout << *it << "|";
			i++;
	}
	std::cout << "~~~vec size: " << i << "\n";
	qu.pop();
	}
}

void parseFile(char *fileName, std::vector<ServerConf> &conf)
{
	(void)conf;
	std::queue<std::vector<std::string> >	qu;

	saveFile(fileName, qu);

	// debugPrintQ(qu);

	configParse(qu, conf);
	
	
}
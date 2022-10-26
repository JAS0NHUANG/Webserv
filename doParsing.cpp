#include "webserv.hpp"

void doServerParsing(std::queue<std::vector<std::string> > &qu, int &line) {
	if (qu.front()[0] != "server")
		throwParsingError(qu.front()[0], toString(line), UNEXPECTED);
		// add: " Expected "server" "

	eraseToken(qu, line);

	if (!isOpenBracket(qu.front()[0]))
		throwParsingError(qu.front()[0], toString(line), UNEXPECTED);
		//add: " Expected "{" "

	eraseToken(qu, line);
	
}

void doListenParsing(std::queue<std::vector<std::string> > &qu, ServerConf &conf, int &line) {
	std::vector<std::string> vec;
	eraseToken(qu, line);
	(void)conf;
	std::cout << "doListenParsing\n";
	while (qu.size() && !isSemicolon(qu.front()[0])) {
		eraseToken(qu, line);
	}
	if (qu.size() == 0)
		throwParsingError(";", toString(line), EXPECTED);
	eraseToken(qu, line);
}

void doServerNameParsing(std::queue<std::vector<std::string> > &qu, ServerConf &conf, int &line) {
	eraseToken(qu, line);
	(void)conf;
	while (qu.size() && !isSemicolon(qu.front()[0])) {
		eraseToken(qu, line);
	}
	if (qu.size() == 0)
		throwParsingError(";", toString(line), EXPECTED);
	eraseToken(qu, line);
}

void doErrorPageParsing(std::queue<std::vector<std::string> > &qu, ServerConf &conf, int &line) {
	eraseToken(qu, line);
	(void)conf;
	while (qu.size() && !isSemicolon(qu.front()[0])) {
		eraseToken(qu, line);
	}
	if (qu.size() == 0)
		throwParsingError(";", toString(line), EXPECTED);
	eraseToken(qu, line);
}

void doClientMaxBodySizeParsing(std::queue<std::vector<std::string> > &qu, ServerConf &conf, int &line) {
	eraseToken(qu, line);
	(void)conf;
	while (qu.size() && !isSemicolon(qu.front()[0])) {
		eraseToken(qu, line);
	}
	if (qu.size() == 0)
		throwParsingError(";", toString(line), EXPECTED);
	eraseToken(qu, line);
}

void doAllowMethodParsing(std::queue<std::vector<std::string> > &qu, ServerConf &conf, int &line) {
	eraseToken(qu, line);
	(void)conf;
	while (qu.size() && !isSemicolon(qu.front()[0])) {
		eraseToken(qu, line);
	}
	if (qu.size() == 0)
		throwParsingError(";", toString(line), EXPECTED);
	eraseToken(qu, line);
}

void doReturnParsing(std::queue<std::vector<std::string> > &qu, ServerConf &conf, int &line) {
	eraseToken(qu, line);
	(void)conf;
	while (qu.size() && !isSemicolon(qu.front()[0])) {
		eraseToken(qu, line);
	}
	if (qu.size() == 0)
		throwParsingError(";", toString(line), EXPECTED);
	eraseToken(qu, line);
}

void doRootParsing(std::queue<std::vector<std::string> > &qu, ServerConf &conf, int &line) {
	eraseToken(qu, line);
	(void)conf;
	while (qu.size() && !isSemicolon(qu.front()[0])) {
		eraseToken(qu, line);
	}
	if (qu.size() == 0)
		throwParsingError(";", toString(line), EXPECTED);
	eraseToken(qu, line);
}

void doAutoindexParsing(std::queue<std::vector<std::string> > &qu, ServerConf &conf, int &line) {
	eraseToken(qu, line);
	(void)conf;
	while (qu.size() && !isSemicolon(qu.front()[0])) {
		eraseToken(qu, line);
	}
	if (qu.size() == 0)
		throwParsingError(";", toString(line), EXPECTED);
	eraseToken(qu, line);
}

void doIndexParsing(std::queue<std::vector<std::string> > &qu, ServerConf &conf, int &line) {
	eraseToken(qu, line);
	(void)conf;
	while (qu.size() && !isSemicolon(qu.front()[0])) {
		eraseToken(qu, line);
	}
	if (qu.size() == 0)
		throwParsingError(";", toString(line), EXPECTED);
	eraseToken(qu, line);
}

void doCgiParsing(std::queue<std::vector<std::string> > &qu, ServerConf &conf, int &line) {
	eraseToken(qu, line);
	(void)conf;
	while (qu.size() && !isSemicolon(qu.front()[0])) {
		eraseToken(qu, line);
	}
	if (qu.size() == 0)
		throwParsingError(";", toString(line), EXPECTED);
	eraseToken(qu, line);
}

void doUploadStoreParsing(std::queue<std::vector<std::string> > &qu, ServerConf &conf, int &line) {
	eraseToken(qu, line);
	(void)conf;
	while (qu.size() && !isSemicolon(qu.front()[0])) {
		eraseToken(qu, line);
	}
	if (qu.size() == 0)
		throwParsingError(";", toString(line), EXPECTED);
	eraseToken(qu, line);
}

void doLocationParsing(std::queue<std::vector<std::string> > &qu, ServerConf &conf, int &line) {

	eraseToken(qu, line);
	(void)conf;
	
	throwIfFileIsEmpty("EMPTY !", qu);
	std::pair<std::string, BaseConf> pr(qu.front()[0], BaseConf());
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY !", qu);

	if (!isOpenBracket(qu.front()[0]))
		throwParsingError(qu.front()[0], toString(line), UNEXPECTED);
		// add : expected '{'

	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY !", qu);


	while (qu.size() && !isCloseBracket(qu.front()[0]))
		callDoers(qu, conf, line);		

	if (isFileEmpty(qu)) 
		throwParsingError("}", toString(line), EXPECTED);
		// add : expected '}' to close location directive
	
	eraseToken(qu, line);
}
#include "./incs/webserv.hpp"

// NOTE: Use throw and catch for error (like parsing errors)
int main(int argc, char *argv[])
{
	if (argc != 2) {
		std::cerr << RED << "Configuration file is missing. Please give it as first parameter.\n" << RESET;
		return 1;
	}

	std::vector<Socket> socket_list;
	std::vector<Server> conf;
	std::vector<Server>::iterator it;

	try {
		// get configuration info
		throwParsingError("", "", argv[0], argv[1]);
		throwError("", argv[0]);
		parseFile(argv[1], conf);
		for (it = conf.begin(); it != conf.end(); it++)
			(*it).debug();
		std::cout << YEL << "Number of server : " << conf.size() << "\n" << RESET;

		// create sockets
		for (it = conf.begin(); it != conf.end(); it++) {
			std::cout << GRN <<  "\naddress: " << (*it).get_address() << ", port: " << (*it).get_port() << RESET;
			socket_list.push_back(Socket((*it).get_port(), (*it).get_address()));
		}
		std::cout << GRN << "\nSockets created!\n" << RESET;
	}
	catch (std::string &str) {
		std::cerr << RED << str << "\n" << RESET;
		return 1;
	}

	// run epoll
	run_epoll(socket_list);

	return (0);
}

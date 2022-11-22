#include "webserv.hpp"

std::sig_atomic_t g_shutdown = 1;

void	signal_handler(int signal) {
	if (signal == SIGINT)
		g_shutdown = 0;
}

// NOTE: Use throw and catch for error (like parsing errors)
int main(int argc, char *argv[])
{
	std::signal(SIGINT, signal_handler);
	if (argc != 2) {
		std::cerr << RED << "Configuration file is missing. Please give it as first parameter.\n" << RESET;
		return 1;
	}

	std::vector<Socket> socket_list;
	std::vector<Config> conf;
	std::vector<Config>::iterator it;

	try {
		// get configuration info
		throwParsingError("", "", argv[0], argv[1]);
		throwError("", argv[0]);
		parseFile(argv[1], conf);
		// for (it = conf.begin(); it != conf.end(); it++)
		// 	(*it).debug();
		std::cout << YEL << "Number of server : " << conf.size() << "\n" << RESET;

		// create sockets
		for (it = conf.begin(); it != conf.end(); it++) {
			std::cout << GRN <<  "\naddress: " << (*it).get_address() << ", port: " << (*it).get_port() << "\n" RESET;
			socket_list.push_back(Socket((*it).get_port(), (*it).get_address(), *it));
		}
		std::cout << GRN << "\nSockets created!\n" << RESET;

		// run server
		run_server(socket_list);
	}
	catch (std::string &str) {
		std::cerr << RED << str << "\n" << RESET;
		return 1;
	}

	return (0);
}

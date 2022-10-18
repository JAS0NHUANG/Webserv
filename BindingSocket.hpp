#ifndef BINDINGSOCKET_HPP
# define BINDINGSOCKET_HPP
#include "SimpleSocket.hpp"

class BindingSocket : public SimpleSocket{
    public:
        BindingSocket(int domain, int service, int protocol, int port, u_long interface);
        int connect_to_network(int sock, struct sockaddr_in addr);

};
#endif
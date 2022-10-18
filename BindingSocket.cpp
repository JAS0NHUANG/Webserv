#include "BindingSocket.hpp"

BindingSocket::BindingSocket(int domain, int service, int protocol, int port, u_long interface): SimpleSocket(domain, service, protocol, port, interface){
    test_connection(connect_to_network(get_sock(), get_addr()));
}
int BindingSocket::connect_to_network(int sock, struct sockaddr_in addr) {
    //const struct sockaddr *pt = addr
    return bind(sock,((struct sockaddr *)&addr), sizeof(addr));
}
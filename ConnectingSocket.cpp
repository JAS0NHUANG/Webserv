#include "ConnectingSocket.hpp"

ConnectingSocket::ConnectingSocket(int domain, int service, int protocol, int port, u_long interface): BindingSocket(domain, service, protocol, port, interface){
    test_connection(connect_to_network(get_sock(), get_addr()));
}
int ConnectingSocket::connect_to_network(int sock, struct sockaddr_in addr) {
    return connect(sock, (struct sockaddr_in *)&addr, sizeof(addr));
}
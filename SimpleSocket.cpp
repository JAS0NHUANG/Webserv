#include "SimpleSocket.hpp"

SimpleSocket::SimpleSocket(int domain, int service, int protocol,int port, u_long interface){
    //define addr structure
    addr.sin_family = domain;
    addr.sin_port =  htons(port);
    addr.sin_addr.s_addr = htonl(interface);
    //establish socket
    sock = socket(domain, service, protocol);
    test_connection(sock);
    //establish network connection
    //connection = connect_to_network(sock, addr);
    //test_connection(connection);
}
void SimpleSocket::test_connection(int item_to_test){
    //std::cout << "|" << item_to_test << "|\n";
    if (item_to_test < 0){
        perror("Error to connect");
        std::cerr << "|" << item_to_test << "|";
        exit(EXIT_FAILURE);
    }
}

struct sockaddr_in SimpleSocket::get_addr(){
    return addr;
}
int SimpleSocket::get_sock(){
    return sock;
}
// int SimpleSocket::get_connection(){
//     return connection;
// }

// void SimpleSocket::set_connection(int con){
//     connection = con;
// }
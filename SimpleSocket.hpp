#ifndef SIMPLESOCKET_HPP
# define SIMPLESOCKET_HPP

#include <sys/socket.h>
#include <netinet/in.h>
// struct addr
#include <iostream>

class SimpleSocket{
    public:
        SimpleSocket(int domain, int service, int protocol, int port, u_long interface);
        virtual int connect_to_network(int sock, struct sockaddr_in addr) = 0; 
        // either call bind() in server or connect in client
        void test_connection(int item_to_test);
        struct sockaddr_in get_addr();
        int get_sock();
        //int get_connection();
        //void set_connection(int con);
    private:
        int sock;
        struct sockaddr_in addr;
        //int connection;

};

#endif
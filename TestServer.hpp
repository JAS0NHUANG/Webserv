#ifndef TESTSERVER_HPP
# define TESTSERVER_HPP

#include "SimpleServer.hpp"
#include <sys/socket.h>
#include <unistd.h>

class TestServer : public SimpleServer{
    private:
        void accepter();
        void handler();
        void responder();
        char buffer[3000] = {0};
        int new_socket;


    public:
        TestServer();      
        void launch();

};
#endif
#ifndef TESTSERVER_HPP
# define TESTSERVER_HPP

#include "SimpleServer.hpp"
#include <sys/socket.h>
#include <unistd.h>

#include <sys/event.h>

#include <fcntl.h> //open


class TestServer : public SimpleServer{
    private:
        void accepter();
        void handler();
        void responder();
        //char buffer[100] = {0};
        int new_socket;


    public:
        TestServer();      
        void launch();

};
#endif
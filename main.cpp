#include <sys/socket.h>
#include <iostream>

#include "SimpleSocket.hpp"

#include "BindingSocket.hpp"
#include "ConnectingSocket.hpp"

#include "ListeningSocket.hpp"

#include "TestServer.hpp"

#include <sys/event.h>
#include <sys/types.h>

int main(){
    // std::cout << "starting ..." << std::endl;
    // std::cout << "Binding ..." << std::endl;
    // BindingSocket bs = BindingSocket(AF_INET, SOCK_STREAM, 0, 4241, INADDR_ANY);
    // std::cout << "Listening ..." << std::endl;
    // ListeningSocket ls = ListeningSocket(AF_INET, SOCK_STREAM, 0, 4243, INADDR_ANY, 20);
    // std::cout << "done ..." << std::endl;

    TestServer ts;


}

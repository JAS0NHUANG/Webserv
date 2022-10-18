#ifndef SIMPLESERVER_HPP
# define SIMPLESERVER_HPP

#include "ListeningSocket.hpp"

class SimpleServer{
    private:
        ListeningSocket *socket;

    public:
        SimpleServer(int domain, int service, int protocol, int port, u_long interface, int bklg);
        virtual void accepter() =0;
        virtual void handler()=0;
        virtual void responder()=0;
        virtual void launch() = 0;
        ListeningSocket *get_socket();

};
#endif
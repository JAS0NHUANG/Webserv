#ifndef LISTENINGSOCKET_HPP
# define LISTENINGSOCKET_HPP
#include "BindingSocket.hpp"

class ListeningSocket : public BindingSocket{
    public:
        ListeningSocket(int domain, int service, int protocol, int port, u_long interface, int bklg);
        void start_listeing();
    private:
        int backlog;
        int listening;

};

#endif
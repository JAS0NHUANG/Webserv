#ifndef _SOCKET_HPP_
# define _SOCKET_HPP_

# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <unistd.h>
# include <iostream>

int create_socket(void);

#endif

#ifndef _SOCKET_HPP_
# define _SOCKET_HPP_

# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <unistd.h>
# include <iostream>

class Socket {
	public:
	// canonical
		Socket(void);
		~Socket(void);
		Socket(const Socket &toCopy);
		Socket &operator=(const Socket &toAssign);

	// member functions
		int	run(void);
	
	private:
};

#endif

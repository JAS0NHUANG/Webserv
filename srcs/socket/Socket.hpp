#ifndef _SOCKET_HPP_
# define _SOCLET_HPP_

# include <sys/socket.h>
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

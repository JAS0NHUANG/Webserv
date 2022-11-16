#include "webserv.hpp"

void	*ft_memset(void *s, int c, size_t n);
void	errMsgErrno(std::string msg);

// epoll
int run_epoll(std::vector<Socket> &socket_list);

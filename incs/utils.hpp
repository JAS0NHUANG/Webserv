#ifndef UTILS_HPP
#define UTILS_HPP

#include "webserv.hpp"

void	*ft_memset(void *s, int c, size_t n);
void	errMsgErrno(std::string msg);
void	throwError(std::string msg, std::string bName = "");

// epoll
int run_epoll(std::vector<Socket> &socket_list);

#endif
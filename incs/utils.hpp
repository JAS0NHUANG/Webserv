#ifndef UTILS_HPP
#define UTILS_HPP

#include "webserv.hpp"

void	*ft_memset(void *s, int c, size_t n);
void	errMsgErrno(std::string msg);
void	throwError(std::string msg, std::string bName = "");
void	str_to_lower(std::string &str);
void	remove(std::string &str, char c);

int		run_server(std::vector<Socket> &socket_list);

#endif
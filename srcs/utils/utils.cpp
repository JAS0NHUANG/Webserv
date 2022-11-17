#include "../../incs/utils.hpp"

void	*ft_memset(void *s, int c, size_t n)
{
	while (n)
	{
		*((unsigned char *)s + n - 1) = (unsigned char)c;
		n--;
	}
	return (s);
}

void	errMsgErrno(std::string msg) {
	std::cerr << RED ;
	std::cerr << msg << ": ";
	std::cerr << strerror(errno) << "\n";
	std::cerr << RESET ;
}

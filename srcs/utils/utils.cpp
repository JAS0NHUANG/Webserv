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
	std::cerr << RED << msg << ": " << strerror(errno) << "\n" << RESET ;
}

void throwError(std::string msg, std::string bName) {
	static std::string binName = bName;
	if (bName != "")
		return;

	throw std::string(binName + ": " + msg + ": " + strerror(errno));
}
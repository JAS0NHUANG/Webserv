#define _BSD_SOURCE

#include <netdb.h>
#include "../includes/is_seqnum.h"

int main(int ac, char **av) {

	char			*reqLenStr;
	char			seqNumStr[INT_LEN];
	int				cfd;
	ssize_t			numRead;
	struct addrinfo *result, *rp;
	struct addrinfo	hints;

	if (ac < 2 || strcmp(av[1], "--help") == 0)
		usageErr("%s server-host [sequence-len]\n", av[0]);
	
	/* Call getaddrinfo() to obtain a list of addresses that
	   we can try connecting to */

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_canonname	= NULL;
	hints.ai_addr		= NULL;
	hints.ai_next		= NULL;
	hints.ai_family		= AF_UNSPEC;	/* Allows IPv4 or Ipv6 */
	hints.ai_socktype	= SOCK_STREAM;
	hints.ai_flags		= AI_NUMERICSERV;

	if (getaddrinfo(av[1], PORT_NUM, &hints, &result) != 0)
		errExit("getaddrinfo");

	/* Walk through returned list until we find an address structure
	   that can be used to successfully connect a socket */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		cfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (cfd == -1)
			continue;	/* On error, try next address */
		
		if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;		/* Success */

		/* Connect failed: close this socket and try next address */

		close(cfd);
	}

	if (rp == NULL)
		fatal("Could not connect socket to any address");

	freeaddrinfo(result);

	/* Send requested sequence length, with terminating newline */

	reqLenStr = (ac > 2) ? av[2] : "1";
	if(write(cfd, reqLenStr, strlen(reqLenStr)) != strlen(reqLenStr))
		fatal("Partial/failed write (reqLenStr)");
	if (write(cfd, "\n", 1) != 1)
		fatal("Partial/failed write (newline)");

	/* Read and display sequence number returned by server */

	numRead = readLine(cfd, seqNumStr, INT_LEN);
	if (numRead == -1)
		errExit("readLine");
	if (numRead == 0)
		fatal("Unexpected EOF from server");

	printf("Sequence number: %s", seqNumStr);	/* Includes '\n' */

	exit(EXIT_SUCCESS);							/* Closes 'cfd' */

}
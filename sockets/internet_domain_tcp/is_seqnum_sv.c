#define _BSD_SOURCE

#include <netdb.h>
#include "../includes/is_seqnum.h"

#define BACKLOG 5

int main(int ac, char **av) {

	uint32_t				seqNum;
	char					reqLenStr[INT_LEN];	/* Length of requested sequence */
	char					seqNumStr[INT_LEN];	/* Start of granted sequence */
	struct sockaddr_storage	claddr;
	int						lfd, cfd, optval, reqLen;
	socklen_t				addrlen;
	struct addrinfo			hints;
	struct addrinfo			*result, *rp;
		#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
	char					addrStr[ADDRSTRLEN];
	char					host[NI_MAXHOST];
	char					service[NI_MAXSERV];

	if (ac > 1 && strcmp(av[1], "--help") == 0)
		usageErr("%s [init-seq-num]\n", av[0]);

	seqNum = (ac > 1) ? getInt(av[1], 0, "init-seq-num") : 0;

	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		errExit("signal");

	/* Call getaddrinfo() to obtain a list of addresses that
	we can try binding to */

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_canonname	= NULL;
	hints.ai_addr		= NULL;
	hints.ai_next		= NULL;
	hints. ai_socktype	= SOCK_STREAM;
	hints.ai_family		= AF_UNSPEC;	/* Allows IPV4 or IPV6 */
	hints.ai_flags		= AI_PASSIVE | AI_NUMERICSERV;
						/* Wildcard IP address; service name is numeric */
	if (getaddrinfo(NULL, PORT_NUM, &hints, &result) != 0)
		errExit("getaddrinfo");

	/* Walk through returned list until we find an address structure
	   that we can be used to successfully create and bind a socket */

	optval = 1;
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		lfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (lfd == -1)
			continue; /* On error try next address */
		if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
			errExit("setsockopt");
		
		if (bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;

		/* bind() failed: close this socket and try next address */

		close(lfd);
	}

	if (rp == NULL)
		fatal("Could not bind socket to any address");

	if (listen(lfd, BACKLOG) == -1)
		errExit("listen");
	
	freeaddrinfo(result);

	for (;;) {						/* Handle clients iteratively */

		/* Accept a client connection, obtaining client's address */

		addrlen	= sizeof(struct sockaddr_storage);
		cfd		= accept(lfd, (struct sockaddr *) &claddr, &addrlen);
		if (cfd == -1) {
			errMsg("accept");
			continue;
		}

		if (getnameinfo((struct sockaddr *) &claddr, addrlen,
			host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
			snprintf(addrStr, ADDRSTRLEN, "(%s, %s)", host, service);
		else
			snprintf(addrStr, ADDRSTRLEN, "(?UNKNOWN?)");
		printf("Connection from %s\n", addrStr);

		/* Read client request, send sequence number back */

		if (readLine(cfd, reqLenStr, INT_LEN) <= 0) {
			close(cfd);
			continue; 				/* Failed read; skip request */
		}

		reqLen = atoi(reqLenStr);
		if (reqLen <= 0) {			/* Watch for misbehaving clients */
			close(cfd);
			continue;				/* Bad request; skip it */
		}

		snprintf(seqNumStr, INT_LEN, "%d\n", seqNum);
		if (write(cfd, &seqNumStr, strlen(seqNumStr)) != strlen(seqNumStr))
			fprintf(stderr, "Error on write");
		
		seqNum += reqLen;			/* Update sequence number */

		if (close(cfd) == -1)		/* Close connection */
			errMsg("close");
	}
}
#include "../includes/i6d_ucase.h"

int main(int ac, char **av) {
	
	struct sockaddr_in6	svaddr;
	int					sfd, j;
	size_t				msgLen;
	ssize_t				numBytes;
	char				resp[BUF_SIZE];

	if (ac < 3 || strcmp(av[1], "--help") == 0)
		usageErr("%s host-address msg...\n", av[0]);
	
	sfd = socket(AF_INET6, SOCK_DGRAM, 0);		/* Create client socket */
	if (sfd == -1)
		errExit("socket");
	
	memset(&svaddr, 0, sizeof(struct sockaddr_in6));
	svaddr.sin6_family	= AF_INET6;
	svaddr.sin6_port	= htons(PORT_NUM);
	if (inet_pton(AF_INET6, av[1], &svaddr.sin6_addr) <= 0)
		fatal("inet_pton failed for address '%s'", av[1]);

	/* Send messages to server; echo responses on stdout */

	for (j = 2; j < ac; j++) {
		msgLen = strlen(av[j]);
		if (sendto(sfd, av[j], msgLen, 0, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_in6)) != msgLen)
			fatal("sento");
		
		numBytes = recvfrom(sfd, resp, BUF_SIZE, 0, NULL, NULL);
		if (numBytes == -1)
			errExit("recvfrom");
		
		printf("Response %d: %.*s\n", j - 1, (int) numBytes, resp);
	}
	exit(EXIT_SUCCESS);
}
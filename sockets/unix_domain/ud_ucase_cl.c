#include "../includes/us_xfr.h"

int main(int ac, char **av) {

	struct	sockaddr_un	svaddr, claddr;
	int					sfd, j;
	size_t				msgLen;
	ssize_t				numBytes;
	char				resp[BUF_SIZE];

	sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sfd == -1)
		errExit("socket");
	

	memset(&claddr, 0, sizeof(struct sockaddr_un));
	claddr.sun_family = AF_UNIX;
	snprintf(claddr.sun_path, sizeof(claddr.sun_path), "/tmp/ud_case_cl%ld", (long) getpid());

	if (bind(sfd, (struct sockaddr *) &claddr, sizeof(struct sockaddr_un)) == -1)
		errExit("bind");

	/* Construct address of server */
	memset(&svaddr, 0, sizeof(struct sockaddr_un));
	svaddr.sun_family = AF_UNIX;
	strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

	/* Send messages to server; echo responses on stdout */
	for (j = 1; j < ac; j++) {
		msgLen = strlen(av[j]);
		if (sendto(sfd, av[j], msgLen, 0, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) != msgLen)
			fatal("sento");

		numBytes = recvfrom(sfd, resp, BUF_SIZE, 0, NULL, NULL);
		if (numBytes == -1)
			errExit("recvfrom");
		printf("Response %d: %.*s\n", j, (int)numBytes, resp);
	}

	remove(claddr.sun_path);
	exit(EXIT_SUCCESS);

	return 0;
}
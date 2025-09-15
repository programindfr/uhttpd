#include "handle.h"
#include "tcp.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void
usage(char *name)
{
	fprintf(stderr, "Usage: %s [-f path] [-p port]\n", name);
}

int
main(int argc, char *argv[])
{
	int   opt;
	int   loop = 1;
	int   sockfd;
	int   peerfd;
	char  *optpath = ".";
	char  *optport = "8000";
	pid_t pid;

	while ((opt = getopt(argc, argv, "f:p:")) != -1)
	{
		switch (opt)
		{
		case 'f':
			optpath = optarg;
			break;
		
		case 'p':
			optport = optarg;
			break;

		default:
			usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	signal(SIGCHLD, SIG_IGN);
	
	sockfd = tcp_serverListen(optport, 2);
	if (sockfd == -1)
		exit(EXIT_FAILURE);

	while (loop)
	{
		peerfd = tcp_serverAccept(sockfd);
		if (peerfd == -1) {
			close(sockfd);
			exit(EXIT_FAILURE);
		}

		pid = fork();
		switch (pid)
		{
		case -1:
			loop = 0;
			break;

		case 0:
			hdl_peer(peerfd, optpath);
			close(peerfd);
			_exit(EXIT_SUCCESS);
			break;

		default:
			break;
		}

		close(peerfd);
	}
	
	close(sockfd);
	
	exit(EXIT_SUCCESS);
}

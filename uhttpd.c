#include "handle.h"
#include "tcp.h"
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int
main(void)
{
	int   loop = 1;
	int   sockfd;
	int   peerfd;
	pid_t pid;

	signal(SIGCHLD, SIG_IGN);
	
	sockfd = tcp_serverListen("8000", 2);
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
			hdl_peer(peerfd);
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

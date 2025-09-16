#include "handle.h"
#include "tcp.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void
version(void)
{
	puts("uhttpd 0.2");
}

void
usage(FILE *stream)
{
	fprintf(
		stream,
		"Usage: uhttpd [OPTION]\n"
		"A micro http server.\n"
		"\n"
		"  -v      : output version and exit\n"
		"  -h      : show help\n"
		"  -f PATH : set the path to serve\n"
		"  -p PORT : set the port to listen to\n"
		"\n"
		"uhttpd  Copyright (C) 2025  Idyie\n"
		"This program comes with ABSOLUTELY NO WARRANTY.\n"
		"This is free software, and you are welcome to\n"
		"redistribute it under certain conditions.\n"
	);
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

	while ((opt = getopt(argc, argv, "vhf:p:")) != -1)
	{
		switch (opt)
		{
		case 'v':
			version();
			exit(EXIT_SUCCESS);
			break;

		case 'h':
			usage(stdout);
			exit(EXIT_SUCCESS);
			break;
		
		case 'f':
			optpath = optarg;
			break;
		
		case 'p':
			optport = optarg;
			break;

		default:
			usage(stderr);
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

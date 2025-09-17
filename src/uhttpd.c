#include "handle.h"
#include "tcp.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void
version(void)
{
	puts("uhttpd 0.3");
}

void
usage(FILE *stream)
{
	fprintf(
		stream,
		"Usage: uhttpd [OPTION]\n"
		"A micro http web server.\n"
		"\n"
		"  -v      : output version and exit\n"
		"  -h      : show help\n"
		"  -d      : run as daemon\n"
		"  -f PATH : set the path to serve\n"
		"  -p PORT : set the listening port\n"
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
	int   daemonflag = 0;
	char  *optpath = "./www";
	char  *optport = "8000";
	pid_t pid;

	while ((opt = getopt(argc, argv, "vhdf:p:")) != -1)
	{
		switch (opt)
		{
		case 'v':
			version();
			goto exit_success;
			break;

		case 'h':
			usage(stdout);
			goto exit_success;
			break;

		case 'd':
			daemonflag = 1;
			break;
		
		case 'f':
			optpath = optarg;
			break;
		
		case 'p':
			optport = optarg;
			break;

		default:
			usage(stderr);
			goto exit_failure;
		}
	}

	if (daemonflag)
	{
		if (daemon(1, 1))
		{
			fprintf(stderr, "daemon: %d\n", errno);
			exit(EXIT_FAILURE);
		}
	}
		
	signal(SIGCHLD, SIG_IGN);
	
	sockfd = tcp_serverListen(optport, 2);
	if (sockfd == -1)
		goto exit_failure;

	while (loop)
	{
		peerfd = tcp_serverAccept(sockfd);
		if (peerfd == -1) {
			close(sockfd);
			goto exit_failure;
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

exit_success:;
	if (daemonflag)
		_exit(EXIT_SUCCESS);
	exit(EXIT_SUCCESS);

exit_failure:;
	if (daemonflag)
		_exit(EXIT_FAILURE);
	exit(EXIT_FAILURE);
}

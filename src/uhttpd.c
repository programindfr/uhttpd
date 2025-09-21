#include "handle.h"
#include "tcp.h"
#include "uhttpd.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void
version(void)
{
	puts("uhttpd " VERSION);
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
		"  -o FILE : output stdout to FILE\n"
		"  -e FILE : output stderr to FILE\n"
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
	int   daemonflag;
	char  *optpath;
	char  *optport;
	pid_t pid;

	daemonflag = OPT_DAEMON;
	optpath = OPT_PATH;
	optport = OPT_PORT;
	
	while ((opt = getopt(argc, argv, "vhdf:p:o:e:")) != -1)
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

		case 'd':
			daemonflag = ~daemonflag;
			break;
		
		case 'f':
			optpath = optarg;
			break;
		
		case 'p':
			optport = optarg;
			break;

		case 'o':
			if (!freopen(optarg, "a", stdout))
				exit(EXIT_FAILURE);
			break;
		
		case 'e':
			if (!freopen(optarg, "a", stderr))
				exit(EXIT_FAILURE);
			break;

		default:
			usage(stderr);
			exit(EXIT_FAILURE);
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

/*exit_success:;*/
	if (daemonflag)
		_exit(EXIT_SUCCESS);
	exit(EXIT_SUCCESS);

exit_failure:;
	if (daemonflag)
		_exit(EXIT_FAILURE);
	exit(EXIT_FAILURE);
}

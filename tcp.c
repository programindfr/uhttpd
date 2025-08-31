#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int
tcp_serverListen(const char *port, int max_accept)
{
	int			 status;
	int			 sockfd;
	struct addrinfo *addr;
	struct addrinfo addr_hints;
	
	memset(&addr_hints, 0, sizeof(addr_hints));
	addr_hints.ai_family = AF_INET;
	addr_hints.ai_socktype = SOCK_STREAM;
	addr_hints.ai_flags = AI_PASSIVE;
	addr_hints.ai_protocol = 0;
	
	status = getaddrinfo(NULL, port, &addr_hints, &addr);
	if (status) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return -1;
	}
	
	sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (sockfd == -1) {
		fprintf(stderr, "socket: %d\n", errno);
		return -1;
	}
	
	status = bind(sockfd, addr->ai_addr, addr->ai_addrlen);
	if (status == -1) {
		fprintf(stderr, "bind: %d\n", errno);
		return -1;
	}
	
	freeaddrinfo(addr);
	
	status = listen(sockfd, max_accept);
	if (status == -1) {
		fprintf(stderr, "listen: %d\n", errno);
		return -1;
	}

	return sockfd;
}

int
tcp_serverPollin0(int sockfd)
{
	struct pollfd sock;

	sock.fd = sockfd;
	sock.events = POLLIN;

	poll(&sock, 1, 0);

	return sock.revents;
}

int
tcp_serverAccept(int sockfd)
{
	int					 peerfd;
	socklen_t			   peer_addrlen;
	struct sockaddr_storage peer_addr;
	
	peer_addrlen = sizeof(peer_addr);
	
	peerfd = accept(sockfd, (struct sockaddr *) &peer_addr, &peer_addrlen);
	if (peerfd == -1) {
		fprintf(stderr, "accept: %d\n", errno);
		return -1;
	}

	return peerfd;
}

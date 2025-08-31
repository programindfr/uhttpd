#ifndef __TCP_H__
#define __TCP_H__

int tcp_serverListen(const char *port, int max_accept);
int tcp_serverPollin0(int sockfd);
int tcp_serverAccept(int sockfd);

#endif/*__TCP_H__*/

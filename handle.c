#include "darray.h"
#include "handle.h"
#include "http.h"
#include "tcp.h"
#include "tokenizer.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/openat2.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

void
hdl_peer(int peerfd)
{
	int  filefd;
	int  code;
	char *request;

	request = hdl_peerRequest(peerfd);
	filefd = hdl_peerTarget(request);
	code = hdl_peerCode(filefd);

	hdl_peerResponse(peerfd, request, filefd, code);

	free(request);
	if (filefd >= 0)
		close(filefd);
}

char *
hdl_peerRequest(int peerfd)
{
	char c;

	DA_ARRAY(char, buffer);

	while (tcp_serverPollin0(peerfd) == POLLIN)
	{
		read(peerfd, &c, 1);
		DA_APPEND(char, buffer, c);
	}

	DA_APPEND(char, buffer, '\0');

	return buffer.array;
}

int
hdl_peerTarget(const char *request)
{
	int			 dirfd;
	int			 filefd;
	char			*target;
	struct open_how how = { 0 };
	
	target = tkz_requestGetHeaderTarget(request);
	write(STDOUT_FILENO, target, strlen(target));
	write(STDOUT_FILENO, "\n", 1);

	how.flags = O_RDONLY;
	how.resolve = RESOLVE_IN_ROOT | RESOLVE_NO_MAGICLINKS;
	dirfd = open(".", O_RDONLY);
	filefd = syscall(SYS_openat2, dirfd, target, &how, sizeof(struct open_how));

	free(target);
	close(dirfd);

	return filefd;
}

int
hdl_peerCode(int filefd)
{
	int		 code;
	struct stat statbuf;

	if (filefd == -1)
	{
		switch (errno)
		{
		case 2:  code = 404; break;
		default: code = 500; break;
		}
	}
	else
	{
		fstat(filefd, &statbuf);
		switch (statbuf.st_mode & S_IFMT)
		{
		case S_IFREG:/*file*/
			if (statbuf.st_mode & S_IXOTH)/*cgi*/
				code = 0;
			else if (statbuf.st_mode & S_IROTH)/*http*/
				code = 200;
			else/*404*/
				code = 404;
			break;

		case S_IFDIR:/*dir*/
			code = 404;
			break;

		default:/*500*/
			code = 500;
			break;
		}
	}

	return code;
}

void
hdl_peerResponse(int peerfd, const char *request, int filefd, int code)
{
	int  fildes[2];
	char c;
	char *header;
	char *body;
	char *newargv[2] = { "", NULL };
	char *newenvp[1] = { NULL };

	header = http_header(code);

	switch(code)
	{
	case 0:/*cgi*/
		pipe(fildes);
		write(fildes[1], request, strlen(request));
		close(fildes[1]);
		dup2(fildes[0], STDIN_FILENO);
		dup2(peerfd, STDOUT_FILENO);
		fexecve(filefd, newargv, newenvp);
		perror("fexecve");
		break;

	case 200:/*http*/
		write(peerfd, header, strlen(header));
		
		while (read(filefd, &c, 1) > 0)
			write(peerfd, &c, 1);
		break;

	default:/*error*/
		body = http_strOfCode(code);
		
		write(peerfd, header, strlen(header));
		write(peerfd, body, strlen(body));
		break;
	}

	free(header);
}

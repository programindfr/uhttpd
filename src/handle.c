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

DA_INIT_NEW(char, char)
DA_INIT_DELETE(char, char)
DA_INIT_APPEND(char, char)

void
hdl_peer(int peerfd, const char *optpath)
{
	int			 filefd;
	int			 code;
	da_array_char_t *request;

	request = hdl_peerRequest(peerfd);
	filefd = hdl_peerTarget(request, optpath);
	code = hdl_peerCode(filefd);

	hdl_peerResponse(peerfd, request, filefd, code);

	da_delete_char(request);
	if (filefd >= 0)
		close(filefd);
}

da_array_char_t *
hdl_peerRequest(int peerfd)
{
	char			c;
	da_array_char_t *buffer;

	buffer = da_new_char();

	while (tcp_serverPollin0(peerfd) == POLLIN)
	{
		read(peerfd, &c, 1);
		da_append_char(buffer, c);
	}

	da_append_char(buffer, '\0');

	return buffer;
}

int
hdl_peerTarget(const da_array_char_t *request, const char *optpath)
{
	int			 dirfd;
	int			 filefd;
	char			*target;
	struct open_how how = { 0 };
	
	target = tkz_requestGetHeaderTarget(request->array);
	write(STDOUT_FILENO, target, strlen(target));
	write(STDOUT_FILENO, "\n", 1);

	how.flags = O_RDONLY;
	how.resolve = RESOLVE_IN_ROOT | RESOLVE_NO_MAGICLINKS;
	dirfd = open(optpath, O_RDONLY);
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
hdl_peerResponse(int peerfd, const da_array_char_t *request, int filefd, int code)
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
		write(fildes[1], request->array, request->length);
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

#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *
http_strOfCode(int code)
{
	switch (code)
	{
	case 200: return "200 OK";
	case 404: return "404 Not Found";
	case 500: return "500 Internal Server Error";
	default:  return "501 Not Implemented";
	}
}

char *
http_header(int code)
{
	char *header;

	header = calloc(HEADER_MAX_SIZE, 1);
	snprintf(
		header,
		HEADER_MAX_SIZE,
		"HTTP/1.1 %s\r\n"
		"Content-Type: text/html; charset=utf-8\r\n"
		"Connection: close\r\n"
		"\r\n",
		http_strOfCode(code)
	);
	header = realloc(header, strlen(header) + 1);

	return header;
}

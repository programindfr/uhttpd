#include "tokenizer.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

char *
tkz_requestGetHeader(const char *request)
{
	char   *str = NULL;
	char   *p;
	size_t len;

	p = strstr(request, "\r\n\r\n");
	len = (size_t) (p - request);
	str = calloc(len + 1, 1);
	memcpy(str, request, len);

	return str;
}

char *
tkz_requestGetBody(const char *request)
{
	char   *str = NULL;
	char   *p;
	size_t len;

	p = strstr(request, "\r\n\r\n") + 4;
	if (p)
	{
		len = strlen(p);
		str = calloc(len + 1, 1);
		memcpy(str, p, len);
	}

	return str;
}

char *
tkz_requestGetHeaderMethod(const char *request)
{
	char   *str = NULL;
	char   *p;
	size_t len;

	p = strstr(request, " ");
	len = (size_t) (p - request);
	str = calloc(len + 1, 1);
	memcpy(str, request, len);

	return str;
}

char *
tkz_requestGetHeaderTarget(const char *request)
{
	char   *str = NULL;
	char   *ps, *pe;
	size_t len;

	ps = strstr(request, " ") + 1;
	pe = strstr(ps, " ");
	len = (size_t) (pe - ps);
	str = calloc(len + 1, 1);
	memcpy(str, ps, len);

	return str;
}

char *
tkz_requestGetHeaderVersion(const char *request)
{
	char   *str = NULL;
	char   *ps, *pe;
	size_t len;

	ps = strstr(request, " ") + 1;
	ps = strstr(ps, " ") + 1;
	pe = strstr(ps, "\r\n");
	len = (size_t) (pe - ps);
	str = calloc(len + 1, 1);
	memcpy(str, ps, len);

	return str;
}

char *
tkz_requestGetHeaderFields(const char *request)
{
	char   *str = NULL;
	char   *p;
	char   *header;
	size_t len;

	header = tkz_requestGetHeader(request);
	
	p = strstr(header, "\r\n") + 2;
	if (p)
	{
		len = strlen(p);
		str = calloc(len + 1, 1);
		memcpy(str, p, len);
	}

	free(header);

	return str;
}

char *
tkz_requestGetHeaderFieldsField(const char *request, const char *field)
{
	char   *str = NULL;
	char   *ps, *pe;
	char   *fields;
	size_t len;

	fields = tkz_requestGetHeaderFields(request);

	ps = strstr(fields, field);
	if (ps)
	{
		ps = strstr(ps, ":") + 1;
		if (*ps == ' ')
			ps++;

		pe = strstr(ps, "\r\n");
		len = (size_t) (pe - ps);
		str = calloc(len + 1, 1);
		memcpy(str, ps, len);
	}

	free(fields);

	return str;
}

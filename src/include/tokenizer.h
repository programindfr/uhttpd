#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

char *tkz_requestGetHeader(const char *request);
char *tkz_requestGetBody(const char *request);

char *tkz_requestGetHeaderMethod(const char *request);
char *tkz_requestGetHeaderTarget(const char *request);
char *tkz_requestGetHeaderVersion(const char *request);
char *tkz_requestGetHeaderFields(const char *request);

char *tkz_requestGetHeaderFieldsField(const char *request, const char *field);

#endif/*__TOKENIZER_H__*/

#ifndef __DARRAY_H__
#define __DARRAY_H__

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define DA_ARRAY(TYPE, NAME) \
struct { \
	TYPE   *array; \
	size_t length; \
} NAME = { 0 }

#define DA_FREE(NAME) \
free(NAME.array); \
NAME.array = NULL; \
NAME.length = 0;

#define DA_APPEND(TYPE, NAME, ELEM) \
NAME.array = realloc(NAME.array, ++NAME.length * sizeof(TYPE)); \
NAME.array[NAME.length - 1] = ELEM

#define DA_REMOVE(TYPE, NAME, ELEM) \
for (size_t i = 0; i < NAME.length; ++i) \
{ \
	if (NAME.array[i] == ELEM) \
	{ \
		if (i < --NAME.length) \
			memmove(&NAME.array[i], &NAME.array[i + 1], (NAME.length - i) * sizeof(TYPE)); \
		NAME.array = realloc(NAME.array, NAME.length * sizeof(TYPE)); \
		break; \
	} \
}

#endif/*__DARRAY_H__*/

#include <stdio.h>
#include "common.h"

void* reallocate(void* pointer, size_t old_size, size_t new_size)
{
	if (new_size == 0)
	{
		free(pointer);
		return NULL;
	}

	void* result = realloc(pointer, new_size);
	return result;
}

int strToInt(char* s)
{
	int nr = 0;
	int n = strlen(s) - 2;
	for(int i = n - 1; i >= 0; i--)
	{
		nr *= 10;
		nr += (s[i] - '0');
	}

	return nr;
}

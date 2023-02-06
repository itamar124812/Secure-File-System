#include "Util.h"
#include <stdlib.h>
#include<time.h>
int randomAsciiString(int size, char** result, char start, char end) {
	srand(time(NULL));
	if (size < 0) return -1;
	if (end < start + 1 || end > 128 || start < 0)return -2;
	if (size == 0) {
		*result = NULL;
		return 0;
	}
	*result = (char*)malloc(size+1);
	for (int i = 0; i < size; i++)
	{
		(*result)[i] = rand() % (end - start) + start;
	}
	(*result)[size] = '\0';
	return 0;
}
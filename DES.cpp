#include <stdlib.h>
#include<string.h>
#include "CustomTypes.h"


byte* ECB(byte* buffer,int length,int blockLen, byte *(*encryptionFunc)(byte*,int)) {
	if (buffer == NULL || length <= 0 || blockLen <= 0 || encryptionFunc == NULL) {
		perror("INVALID INPUT.");
		return NULL;
	}


	int blocksNum, shiftNum;
	byte* temp, *result;


	blocksNum = length / blockLen;
	shiftNum = length % blockLen;
	if (shiftNum > 0) {
		temp = (byte*)realloc(buffer, length + shiftNum);
		if (!temp) {
			perror("Unable to properly pad the buffer due to memory issues");
		}
		memset(buffer+length, 0, shiftNum);
		blocksNum++;
	}

	result = (byte*) malloc(blocksNum * blockLen);
	for (int i = 0; i < blocksNum; i++)
	{
		encryptionFunc(buffer+i*blockLen, blockLen);
	}

}
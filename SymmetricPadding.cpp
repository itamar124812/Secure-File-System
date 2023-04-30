#include "SymmetricPadding.h"
#include<stdlib.h>
#include<string.h>
byte* padBuffer(byte* buffer, int bufferLen, int blockLen, PaddingMethods paddingMethod, int* resultLen) {

	if (buffer == NULL || bufferLen <= 0 || blockLen <= 0) {
		perror("INVALID INPUT. Length must be bigger then ten.");
		return NULL;
	}

	byte* res;
	int blocksNum, remainder, paddingSize;


	blocksNum = bufferLen / blockLen + 1;
	remainder = bufferLen % blockLen;
	paddingSize = remainder > 0 ? remainder : blockLen;
	res = (byte*)realloc(buffer, bufferLen + blockLen);
	if (!res) {
		perror("Unable to properly pad the buffer due to memory issues.");
		return NULL;
	}
	switch (paddingMethod)
	{
	case PKCS7:
		memset(res + bufferLen, paddingSize, paddingSize);
		break;
	case ONEBYZEROS:
		res[bufferLen] = (byte)1;
		memset(res + bufferLen + 1, 0, paddingSize - 1);
		break;
	case ANSIX923:
		memset(res + bufferLen, 0, paddingSize - 1);
		res[bufferLen + paddingSize - 1] = paddingSize;
		break;
	case ISO10126:
		res[bufferLen + paddingSize - 1] = paddingSize;
		break;
	default:
		break;
	}
	*resultLen = bufferLen + paddingSize;
	return res;
}
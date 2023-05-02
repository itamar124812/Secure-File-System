#include <stdlib.h>
#include<string.h>
#include "CustomTypes.h"
#include "SymmetricPadding.h"


byte* ECB(byte* buffer,int length,int blockLen,PaddingMethods paddingMethod, byte *(*encryptionFunc)(byte*,int)) {
	const char* errorString;
	int resLength = 0, blocksNum;
	byte* temp, * result;
	if (buffer == NULL || length <= 0 || blockLen <= 0 || encryptionFunc == NULL) {
	errorString ="INVALID INPUT.";
	goto err;
	}
	temp = padBuffer(buffer, length, blockLen, paddingMethod, &resLength);
	if (!temp) {
		errorString = "An error has occurred while padding.";
		goto err;
	}
	buffer = temp;
	result = (byte*) malloc(resLength*sizeof(byte));
	if (!result) goto err;
	blocksNum = resLength / blockLen;
	for (int i = 0; i < blocksNum; i++)
	{
		temp=encryptionFunc(buffer+i*(size_t)blockLen, blockLen);
		if (!temp) {
			errorString = "An error has occurred while encrypting.";
			goto err;
		}
		memcpy(result + i * (size_t)blockLen, temp, blockLen);
	}
	return result;
err:
	perror(errorString);
	return NULL;
}
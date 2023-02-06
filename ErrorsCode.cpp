#include "ErrorsCode.h"
#include <stdio.h>
void printErrorsMessages(int errorCode)
{
	switch (errorCode)
	{
	case BadMalloc:
		perror("error when trying to allocate buffer");
		break;
	case BadParams:
		fputs("invalid params to function.", stderr);
		break;
	case InvalidKeySize:
		fputs("invalid key size.", stderr);
		break;
	case InvalidKey:
		fputs("invalid key.", stderr);
		break;



	default:
		fputs("unknown error.", stderr);
		break;
	}
}
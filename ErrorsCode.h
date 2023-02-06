#pragma once
#include<errno.h>
#include<stdlib.h>
#define OperationFailed -1
#define BadMalloc -2
#define BadParams -10
#define InvalidKeySize -20
#define InvalidKey -21
#define NotTrustedResult -22


void printErrorsMessages(int errorCode);

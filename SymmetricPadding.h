#pragma once
#include "CustomTypes.h"
typedef enum
{
	PKCS7_P,
	ONEBYZEROS,
	ANSIX923,
	ISO10126

} PaddingMethods;
byte* padBuffer(byte* buffer, int bufferLen, int blockLen, PaddingMethods paddingMethod, int* resultLen);
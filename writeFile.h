#pragma once
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include<stdio.h>
int writeBufferToFile(const unsigned char* buffer, int bufferSize, const char *path);


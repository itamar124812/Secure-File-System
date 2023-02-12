#pragma once
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include<stdio.h>
#include<stdlib.h>

#define CHUNK_SIZE 1024


typedef struct {
	FILE* fp;
	unsigned char chunk[CHUNK_SIZE];
	size_t chunk_size;
	long long Filesize;
} file_chunks;

int init_file_chunks(const char* path, file_chunks* fc, bool binaryFile=false);
int get_next_chunk(file_chunks* fc);
int release_file_chunks(file_chunks* fc);
int readFileAtOnce(const char* path, unsigned char** fileContent, int* size);
long long getFileLength(FILE* file);
long long getWindowsFileLength(FILE* file);
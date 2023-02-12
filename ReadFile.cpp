#include "ReadFile.h"
#include <Windows.h>
#include <io.h>
int readFileAtOnce(const char* path, unsigned char** fileContent, int* size) {
	FILE* f;
	if ((f = fopen(path, "rb")) == NULL) {
		perror("Could not open the file because the following error: ");
		return -1;
	}
	else {
		*size = getFileLength(f);
		if (*size == 0) {
			fclose(f);
			perror("file with size of zero.\n");
			return -2;
		}
		*fileContent = (unsigned char*)malloc((*size) + 1);
		if (*fileContent == NULL) {
			fclose(f);
			perror("null pointer from malloc.\n");
			return -3;
		}
		if (fread(*fileContent, (*size), 1, f) != 1) {
			fclose(f);
			free(fileContent);
			fputs("Reading error", stderr);
			return(-4);
		}
		(*fileContent)[*size] = '\0';
		fclose(f);
	}
	return 0;
}

int init_file_chunks(const char* path, file_chunks* fc,bool binaryFile) {
	if(binaryFile)fc->fp = fopen(path, "rb");
	else fc->fp = fopen(path, "r");	
	if (fc->fp == NULL) {
		perror("fopen");
		return 1;
	}
	fc->chunk_size = 0;
	return 0;
}
int get_next_chunk(file_chunks* fc) {
	size_t total_read = 0;
	size_t remaining = CHUNK_SIZE;
	size_t chunk_size = 0;
	while (total_read < CHUNK_SIZE && !feof(fc->fp)) {
		chunk_size = fread(fc->chunk + total_read, 1, remaining, fc->fp);
		total_read += chunk_size;
		remaining -= chunk_size;
	}
	fc->chunk_size = total_read;
	if (ferror(fc->fp)) {
		perror("fread");
		return 1;
	}
	return (fc->chunk_size > 0);
}


int release_file_chunks(file_chunks* fc) {
	if (fclose(fc->fp) != 0) {
		perror("fclose");
		return 1;
	}
	return 0;
}

long long getFileLength(FILE* file) {
	#ifdef _WIN32
	return getWindowsFileLength(file);
	#else
	if (fseek(file, 0, SEEK_END) != 0) return -1;
	long long size = ftell(file);
	if (size == -1)return -1;
	rewind(file);
	return size;
	#endif
}


long long getWindowsFileLength(FILE* file) {
	int fd = _fileno(file);
	HANDLE handle = (HANDLE)_get_osfhandle(fd);
	_LARGE_INTEGER fileSize;
	memset(&fileSize, 0, sizeof(_LARGE_INTEGER));
	if(GetFileSizeEx(handle, &fileSize)) return fileSize.QuadPart;
	else {
		DWORD dw = GetLastError();
		printf("function faild with this error code: %d", dw);
		return -1;
	}
}
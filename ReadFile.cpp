#include "ReadFile.h"
int readFileAtOnce(const char* path, unsigned char** fileContent, int* size) {
	FILE* f;
	if ((f = fopen(path, "rb")) == NULL) {
		perror("Could not open the file because the following error: ");
		return -1;
	}
	else {
		fseek(f, 0, SEEK_END);
		*size = ftell(f);
		rewind(f);
		if (*size == 0) {
			perror("file with size of zero.\n");
			return -2;
		}
		*fileContent = (unsigned char*)malloc((*size) + 1);
		if (*fileContent == NULL) {
			perror("null pointer from malloc.\n");
			return -3;
		}
		if (fread(*fileContent, (*size), 1, f) != 1) {
			fputs("Reading error", stderr);
			return(-4);
		}
		(*fileContent)[*size] = '\0';
		fclose(f);
	}
	return 0;
}

int init_file_chunks(const char* path, file_chunks* fc) {
	fc->fp = fopen(path, "r");
	if (fc->fp == NULL) {
		perror("fopen");
		return 1;
	}
	fc->chunk_size = 0;
	return 0;
}
int get_next_chunk(file_chunks* fc) {
	fc->chunk_size = fread(fc->chunk, 1, CHUNK_SIZE, fc->fp);
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
	if (fseek(file, 0, SEEK_END) != 0) return -1;
	long long size = ftell(file);
	if (size == -1)return -1;
	rewind(file);
	return size;
}



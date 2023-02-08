#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include<iostream>
#include <openssl/ssl.h>
#include<openssl/aes.h>
#include<openssl/rand.h>
#include <openssl/evp.h>
#include "ReadFile.h"
#include "AES.h"


using namespace std;


void test1();
int main() {
	
	test1();
	system("pause");
	return EXIT_SUCCESS;
}
void test1() {
	AESResult* result = NULL;
	const char* path = R"(C:\Users\USER\Source\Repos\Project22\Debug\longText.txt)";
	encryptFile(path,&result,true,32,32);
	printf("%d",decryptFile(path, result));
	free(result->path);
	free(result->AAD);
	free(result->iv);
	free(result->key);
}
void test() {
	const char* str = "New file.\nWhat a wonderful thing.\n";
	const char* path = "newFile.txt";
	unsigned char* fileContent = NULL;
	int size = 0;
	FILE* f = fopen(path, "w+");
	fprintf(f, str);
	fclose(f);
	if (readFileAtOnce(path, &fileContent, &size) == 0) {
		printf((const char*)fileContent);
	}
	else {
		printf("an error has occurred");
	}
	//free((void*)str);
	//free((void*)path);
	free((void*)fileContent);
}

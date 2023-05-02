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
#include "SymmetricPadding.h"


using namespace std;
void testPadding();
char* toHex(int num) {
	char result[9] = { 0 };
	sprintf(result, "%x", num);
	return result;
}
void test1();
char** bufferToHexString(const unsigned char* buffer, int size); 
void printbufferAsHex(const unsigned char* buffer, int size);
char** bufferToHexString(const unsigned char* buffer, int size) {
	
	char* result =(char*) malloc(size*2+1);
	for (size_t i = 0; i < size; i++)
	{
		sprintf(result + i*2, "%02X", buffer[i]);
		printf("%02X", buffer[i]);
	}
	result[size*2] = 0;
	return &result;
}
void printbufferAsHex(const unsigned char* buffer, int size)
{
	char** s=bufferToHexString(buffer, size);
	printf("%s", *s);
	free((void*)(*s));
}
int main() {
	testPadding();
	printf("%s", toHex(10));
	test1();
	system("pause");
	return EXIT_SUCCESS;
}
void test1() {
	AESResult* result = NULL;
	const char* path = R"(C:\Users\USER\Source\Repos\Project22\Debug\longText.txt)";
	encryptFile(path,&result,true,32,32);
	printf("key: ");
	printbufferAsHex(result->key, result->keySize);
	printf("\nIV: ");
	printbufferAsHex(result->iv, 12);
	printf("\nTag: ");
	printbufferAsHex(result->tag, 16);
	printf("\nAAD: %s\n", result->AAD);
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
void testPadding() {
	const char* someString = "lets play soccer today";
	byte paddingResult[] = "lets play soccer today\n\n\n\n\n\n\n\n\n";
	int length = strlen(someString);
	byte* buffer = (byte*)malloc(sizeof(byte) * length);
	memcpy(buffer, someString, length);
	int res = 0;
	buffer = padBuffer(buffer, length, 16, PKCS7_P, &res);
	if (res != 32) {
		printf("the length should be 32.");
			goto clear_area;
	}
	if(memcmp(paddingResult, buffer, 32)) printf("the padding should be %s",paddingResult);
	clear_area:
	free(buffer);
}
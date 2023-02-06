#pragma once
#include <openssl/rand.h>
#include "ReadFile.h" 
#include <string.h>
#include "writeFile.h"
#include "ErrorsCode.h"
#define IVSize  12
#define MaxAAD 256
enum AESMode{GCM,CCM,CBC, CFB,CFB1,ECB,OFB};
static struct AADParams {
	int userId;
	int keySize; 
	AESMode aesMode = GCM;
	long fileSize;	
};
struct AESResult {
	unsigned char* key;
	unsigned char* iv;
	char* AAD;
	int keySize;
	unsigned char tag [16];
	unsigned char* buffer;
	int length;
};
int encryptFile(const char* path, AESResult * result, bool withAAD=false, int userId = 0, int keySize = 32);
int decryptFile(const char* path, AESResult* aesValuesToDecrypt,int keySize=32);
int encryptBuffer();

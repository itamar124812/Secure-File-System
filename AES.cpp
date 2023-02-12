#include "AES.h"
#include "Util.h"
#include<errno.h>
#include<fstream>


static int generateKey(unsigned char** key, int keySize)
{
	if (keySize != 32 && keySize != 16 && keySize != 24) {
		printErrorsMessages(InvalidKeySize);
		return InvalidKeySize;
	}
	*key = (unsigned char*)malloc(keySize);
	if (*key == NULL) {
		printErrorsMessages(BadMalloc);
		return BadMalloc;
	}
	if (RAND_bytes(*key, keySize) == 1) return 0;
	else {
		free(*key);
		return OperationFailed;
	}
}
static int generateIV(unsigned char** iv) {
	*iv = (unsigned char*)malloc(IVSize);
	if (*iv == NULL) {
		printErrorsMessages(BadMalloc);
		return BadMalloc;
	}
	if (RAND_bytes(*iv, IVSize) == 1)return 0;
	else {
		free(*iv);
		return OperationFailed;
	}
}
static int generateAAD(AADParams* params, char** AAD) {
	if (params == NULL)return BadParams;
	*AAD = (char*)malloc(MaxAAD);
	if (*AAD == NULL)
		return BadMalloc;
	int length = 0;
	char str[200];
	length += snprintf(str, sizeof(str), "userId: %d, file size: %ld, AesMode: %d, KeySize: %d, randomString: ", params->userId, params->fileSize, params->aesMode, params->keySize);
	memcpy(*AAD, str, length);
	int randomStringLength = MaxAAD - length;
	char* randString = NULL;
	randomAsciiString(randomStringLength, &randString, 48, 122);
	memcpy(*AAD + length, randString, randomStringLength);
	(*AAD)[255] = 0;
	free(randString);
	return 0;
}
static void freeAll(AESResult* aesEncryptionResult, EVP_CIPHER_CTX** ctx = NULL, void** ptr = NULL, int count = 0 , file_chunks * fileChank=NULL) {
	if (fileChank != NULL)
		release_file_chunks(fileChank);
	if (ctx != NULL && *ctx != NULL) {
		printf("cleaning ctx");
		EVP_CIPHER_CTX_free(*ctx);
	}
	for (int i = 0; i < count; i++)
	{
		if (ptr[i] != NULL)free(ptr[i]);
	}
	if (aesEncryptionResult != NULL) {
		if (aesEncryptionResult->iv != NULL) free(aesEncryptionResult->iv);
		if (aesEncryptionResult->key != NULL) free(aesEncryptionResult->key);
		if (aesEncryptionResult->AAD != NULL) free(aesEncryptionResult->AAD);
		if (aesEncryptionResult->path != NULL) free(aesEncryptionResult->path);


		free(aesEncryptionResult);
	}
}
static int AESInit(unsigned char** key, unsigned char** iv, int keySize, EVP_CIPHER_CTX** ctx) {
	if (generateIV(iv) != 0) return OperationFailed;
	if (generateKey(key, keySize)) {
		free(iv);
		return OperationFailed;
	}
	*ctx = EVP_CIPHER_CTX_new();
	const EVP_CIPHER* chiper;
	switch (keySize) {
	case 32:
		chiper = EVP_aes_256_gcm();
		break;
	case 16:
		chiper = EVP_aes_128_gcm();
		break;
	case 24:
		chiper = EVP_aes_192_gcm();
		break;
	default:
		printErrorsMessages(InvalidKeySize);
		return InvalidKeySize;
	}
	EVP_EncryptInit_ex(*ctx, chiper, NULL, *key, *iv);
	return 0;
}
int encryptFile(const char* path, AESResult** result, bool withAAD,int userId, int keySize)
{
	void* pointers[10];
	int count = 0;
	*result = (AESResult*)malloc(sizeof(AESResult));
	memset(*result, 0, sizeof(AESResult));
	(*result)->keySize = keySize;
	EVP_CIPHER_CTX* ctx = NULL;
	if (AESInit(&((*result)->key), &((*result)->iv), keySize, &ctx)) {
		fputs("an error has occurred when initialize.", stderr);
		return OperationFailed;
	}

	file_chunks fc = file_chunks{};
	if (init_file_chunks(path, &fc) != 0) {
		fputs("error when trying to open the file.", stderr);
		freeAll((*result), &ctx);
		return EIO;
	}
	(*result)->length = getFileLength(fc.fp);
	if (withAAD) {
		AADParams aadParams = {};
		aadParams.fileSize = (*result)->length;
		aadParams.aesMode = GCM;
		aadParams.keySize = keySize;
		aadParams.userId = userId;
		generateAAD(&aadParams, &((*result)->AAD));
		int len = 0;
		if (1 != EVP_EncryptUpdate(ctx, NULL, &len, (const unsigned char*)(*result)->AAD, strlen((*result)->AAD)))
		{
			freeAll((*result), &ctx, pointers, count,&fc);
			return OperationFailed;
		}
	}
	unsigned char* encryptedFile = (unsigned char*)malloc((int)((*result)->length + 1024));
	if (encryptedFile == NULL)
	{
		freeAll((*result), &ctx, pointers, count,&fc);
		return BadMalloc;
	}
	pointers[count++] = encryptedFile;
	unsigned char* buffer = (unsigned char*)malloc((int)(keySize + CHUNK_SIZE));
	if (buffer == NULL)
	{
		freeAll((*result), &ctx, pointers, count,&fc);
		return BadMalloc;
	}
	pointers[count++] = buffer;
	int resultOut = 0, out = 0;
	while (get_next_chunk(&fc) != 0) {
		if (!EVP_EncryptUpdate(ctx, buffer, &out, (const unsigned char*)fc.chunk, fc.chunk_size)) {
			freeAll((*result), &ctx, pointers, count,&fc);
			free(result);
			fputs("An error has occurred when trying to encrypt the file.\n", stderr);
			return OperationFailed;
		}
		memcpy(encryptedFile + resultOut, buffer, out);
		resultOut += out;
	}
	if (1 == EVP_EncryptFinal_ex(ctx, encryptedFile + resultOut, &out));
	{
		resultOut += out;
	}
	if (writeBufferToFile(encryptedFile, resultOut, path)) {
		freeAll(NULL, &ctx, pointers, count);
		fputs("The encryption process was successful. An error occurred while trying to overwrite the file.\n", stderr);
		return EIO;
	}
	int pathLen = strlen(path);
	(*result)->path =(char *) malloc(pathLen);
	memcpy((*result)->path, path, pathLen);

	if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, &((*result)->tag))) {
		freeAll(NULL, &ctx, pointers, count);
	}
	freeAll(NULL, &ctx, pointers, count);
	return 0;
}
int decryptFile(const char* path, AESResult * aesValuesToDecrypt, int keySize) {
	void* ptr[10];
	int count = 0, out = 0, resultOut = 0;
	bool trusted = true;
	EVP_CIPHER_CTX* ctx;
	ctx = EVP_CIPHER_CTX_new();
	if (!EVP_DecryptInit(ctx, EVP_aes_256_gcm(), aesValuesToDecrypt->key, aesValuesToDecrypt->iv)) {

		return OperationFailed;
	}
	file_chunks fc = file_chunks{};
	if (init_file_chunks(path, &fc,true) != 0) {
		fputs("error when trying to open the file.\n", stderr);
		return EIO;
	}
	if (!EVP_DecryptInit_ex(ctx, NULL, NULL, aesValuesToDecrypt->key, aesValuesToDecrypt->iv)) {
		freeAll(NULL, &ctx, ptr, count, &fc);
		perror("An error has occurred during initializtion.\n");
		return OperationFailed;
	}
	if (aesValuesToDecrypt->AAD != NULL) {
		if (!EVP_DecryptUpdate(ctx, NULL, &out, (const unsigned char *)aesValuesToDecrypt->AAD, MaxAAD)) {
			freeAll(NULL, &ctx, ptr, count, &fc);
			perror("An error has occurred with the AAD.");
			return OperationFailed;
		}
	}
	unsigned char* output =(unsigned char*) malloc((aesValuesToDecrypt->length)+1024);
	ptr[count++] = output;
	out = 0;
	while (get_next_chunk(&fc)) {
		if (!EVP_DecryptUpdate(ctx, output + resultOut, &out, fc.chunk, fc.chunk_size))
		{
			freeAll(NULL, &ctx, ptr, count, &fc);
			perror("An error has occurred during the descryption.\n");
			return OperationFailed;
		}
		resultOut += out;
	}
	
	if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, aesValuesToDecrypt->tag)) {
		fputs("An error has occurred with the tag. The result is untrusted.\n",stderr);
		trusted = false;		 
	}
	if (EVP_DecryptFinal_ex(ctx, output + resultOut, &out) <= 0) {
		fputs("An error has occurred with the final stage. The result is untrusted.\n", stderr);
		trusted = false;
	}
	resultOut += out;
	if (writeBufferToFile(output, resultOut, path)) {
		freeAll(NULL, &ctx, ptr, count, &fc);
		fputs("The decryption process was successful. An error occurred while trying to overwrite the file.\n", stderr);
		return EIO;
	}
	
	aesValuesToDecrypt->length = resultOut;
	freeAll(NULL, &ctx, ptr, count, &fc);
	if (trusted) return 0;
	return NotTrustedResult;
}

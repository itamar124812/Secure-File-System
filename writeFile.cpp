#include "writeFile.h"
int writeBufferToFile(const unsigned char* buffer, int bufferSize, const char * path)
{
    FILE* f=fopen(path, "w+");
    int result = fwrite(buffer, sizeof(char), bufferSize, f);
    if (result != bufferSize) {
        fclose(f);
        fputs("error when writing to file.", stderr);
        return -1;
    }
    return 0;
}
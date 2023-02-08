#include "writeFile.h"
int writeBufferToFile(unsigned char* buffer, int bufferSize, const char * path)
{
    FILE* f=fopen(path, "w+");
    if (f == 0) {
        perror("cann't open the file.");
        return -1;
    }
    int result = fwrite(buffer, sizeof(char), bufferSize, f);
    fclose(f);
    if (result != bufferSize) {  
        fputs("error when writing to file.", stderr);
        return -1;
    }
    return 0;
}
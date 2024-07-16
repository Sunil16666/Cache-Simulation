#ifndef FILE_PROCESSING_H
#define FILE_PROCESSING_H

#include <stdint.h>
#include <stdlib.h>

typedef struct
{
    uint32_t addr;
    uint32_t data;
    int we;
} Request;

typedef struct
{
    char* csvFilePath;
} FileProcessing;

FileProcessing* createFileProcessing(const char* csvFilePath);
void deleteFileProcessing(FileProcessing* fileProc);
void getRequests(const FileProcessing* fileProc, size_t* numRequests, Request** requests);

#endif // FILE_PROCESSING_H

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

/*
    Creates a new FileProcessing object
    parameters:
        csvFilePath: path to the csv file to process
    returns: FileProcessing
*/
FileProcessing* createFileProcessing(const char* csvFilePath);

/*
    Deletes the file Processing object
    parameters:
        FileProcessing the object to destroy
    returns: -
*/
void deleteFileProcessing(FileProcessing* fileProc);

/*
    Gets the requests from the specified csv file
    parameters:
        fileProc: the file processor
        numRequests pointer to where the number of requests will be stored
        requests pointer to where the array of requests will be stored
    returns: -
*/
void getRequests(const FileProcessing* fileProc, size_t* numRequests, Request** requests);

#endif // FILE_PROCESSING_H

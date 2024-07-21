#include "file_processing.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

/*
    Creates a new FileProcessing object
    parameters:
        csvFilePath: path to the csv file to process
    returns: FileProcessing
*/
FileProcessing* createFileProcessing(const char* csvFilePath)
{
    FileProcessing* fileProc = (FileProcessing*)malloc(sizeof(FileProcessing));
    fileProc->csvFilePath = strdup(csvFilePath);
    return fileProc;
}

/*
    Deletes the file Processing object
    parameters:
        FileProcessing the object to destroy
    returns: -
*/
void deleteFileProcessing(FileProcessing* fileProc)
{
    free(fileProc->csvFilePath);
    free(fileProc);
}

/*
    Gets the requests from the specified csv file
    parameters:
        fileProc: the file processor
        numRequests pointer to where the number of requests will be stored
        requests pointer to where the array of requests will be stored
    returns: -
*/
void getRequests(const FileProcessing* fileProc, size_t* numRequests, Request** requests)
{
    FILE* file = fopen(fileProc->csvFilePath, "r");

    //If file fails to open exit program with error
    if (!file)
    {
        fprintf(stderr, "Error opening file: %s\n", fileProc->csvFilePath);
        *numRequests = 0;
        *requests = NULL;
        exit(1);
        return;
    }

    char line[256];
    fgets(line, sizeof(line), file); //Skip first line since it doesn't contain data

    Request* requestArray = NULL;
    size_t requestCount = 0;
    size_t requestCapacity = 0;

    while (fgets(line, sizeof(line), file))
    {
        char type[2];
        unsigned int addr;
        unsigned int data = 0;
        int items;
        char dataStr[256] = {0};

#ifdef DEBUG
        printf("Reading line: %s", line);
#endif

        if (sscanf(line, "%1s,%x,%255s", type, &addr, dataStr) < 2)
        {
            fprintf(stderr, "Failed to parse line: %s\n", line);
            exit(1);
            continue;
        }

        //Type = Write
        if (type[0] == 'W')
        {
            if (sscanf(dataStr, "%u", &data) != 1)
            {
                fprintf(stderr, "Incorrect format for write request: %s\n", line);
                exit(1);
                continue;
            }
        } //Type = Read
        else if (type[0] == 'R')
        {
            if (dataStr[0] != '\0')
            {
                // Throw error if data field is not empty
                fprintf(stderr, "Incorrect format for read request (data should be empty): %s\n", line);
                exit(1);
                continue;
            }
            data = 0; //Ensure data variable is empty for read requests
        }
        else
        {
            fprintf(stderr, "Unknown request type: %s\n", line);
            exit(1);
            continue;
        }

#ifdef DEBUG
        printf("Parsed type: %s, addr: %x, data: %u\n", type, addr, data);
#endif

        if (addr > UINT32_MAX || data > UINT32_MAX)
        {
            fprintf(stderr, "Value exceeds uint32_t limits: %s\n", line);
            continue;
        }

        if (requestCount >= requestCapacity)
        {
            requestCapacity = (requestCapacity == 0) ? 1 : requestCapacity * 2;
            Request* tempArray = (Request*)realloc(requestArray, requestCapacity * sizeof(Request));
            if (tempArray == NULL)
            {
                fprintf(stderr, "Memory allocation failed\n");
                free(requestArray);
                fclose(file);
                *numRequests = 0;
                *requests = NULL;
                return;
            }
            requestArray = tempArray;
        }


        Request req;
        req.addr = addr;
        req.data = data;
        req.we = (type[0] == 'W') ? 1 : 0;
        requestArray[requestCount++] = req;
    }

    fclose(file);

    *numRequests = requestCount;
    *requests = requestArray;
}

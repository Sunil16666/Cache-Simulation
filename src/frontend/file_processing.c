#include "file_processing.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>

FileProcessing* createFileProcessing(const char* csvFilePath)
{
    FileProcessing* fileProc = (FileProcessing*)malloc(sizeof(FileProcessing));
    fileProc->csvFilePath = strdup(csvFilePath);
    return fileProc;
}

void deleteFileProcessing(FileProcessing* fileProc)
{
    free(fileProc->csvFilePath);
    free(fileProc);
}

void getRequests(const FileProcessing* fileProc, size_t* numRequests, Request** requests)
{
    FILE* file = fopen(fileProc->csvFilePath, "r");

    if (!file)
    {
        fprintf(stderr, "Error opening file: %s\n", fileProc->csvFilePath);
        *numRequests = 0;
        *requests = NULL;
        return;
    }

    char line[256];
    fgets(line, sizeof(line), file);

    Request* requestArray = NULL;
    size_t requestCount = 0;
    size_t requestCapacity = 0;

    while (fgets(line, sizeof(line), file))
    {
        char type[2];
        unsigned int addr;
        unsigned int data;
        int items;

        #ifdef DEBUG
        printf("Reading line: %s", line);
        #endif

        if (sscanf(line, "%1s", type) != 1) {
            fprintf(stderr, "Failed to read request type: %s\n", line);
            continue;
        }

        if (type[0] == 'W') {
            items = sscanf(line, "%1s,%x,%u", type, &addr, &data);
            if (items != 3) {
                fprintf(stderr, "Incorrect format for write request: %s\n", line);
                continue;
            }
        } else if (type[0] == 'R') {
            items = sscanf(line, "%1s,%x", type, &addr);
            if (items != 2) {
                fprintf(stderr, "Incorrect format for read request: %s\n", line);
                continue;
            }
            data = 0; // For read requests, data is not used.
        } else {
            fprintf(stderr, "Unknown request type: %s\n", line);
            continue;
        }

        // = sscanf(line, "%1s,%10x,%10u", type, &addr, &data);

        #ifdef DEBUG
        printf("Parsed items: %d\n", items);
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
                free(requestArray); // Prevent memory leak
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

#include "file_processing.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>

FileProcessing* createFileProcessing(const char* csvFilePath) {
    FileProcessing* fileProc = (FileProcessing*)malloc(sizeof(FileProcessing));
    fileProc->csvFilePath = strdup(csvFilePath);
    return fileProc;
}

void deleteFileProcessing(FileProcessing* fileProc) {
    free(fileProc->csvFilePath);
    free(fileProc);
}

void getRequests(const FileProcessing* fileProc, size_t* numRequests, Request** requests) {
    FILE* file = fopen(fileProc->csvFilePath, "r");

    if (!file) {
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

    while (fgets(line, sizeof(line), file)) {
        char type[2];
        char addrStr[11];
        char dataStr[11];

        if (sscanf(line, "%1s,%10s,%10s", type, addrStr, dataStr) != 3) {
            continue;
        }

        unsigned long addr = strtoul(addrStr + 2, NULL, 16);
        unsigned long data = strtoul(dataStr, NULL, 10);

        if (addr > UINT32_MAX || data > UINT32_MAX) {
            fprintf(stderr, "Value exceeds uint32_t limits: %s\n", line);
            continue;
        }

        if (requestCount >= requestCapacity) {
            requestCapacity = (requestCapacity == 0) ? 1 : requestCapacity * 2;
            requestArray = (Request*)realloc(requestArray, requestCapacity * sizeof(Request));
        }

        Request req;
        req.addr = (uint32_t)addr;
        req.data = (uint32_t)data;
        req.we = (type[0] == 'W') ? 1 : 0;
        requestArray[requestCount++] = req;
    }

    fclose(file);

    *numRequests = requestCount;
    *requests = requestArray;
}

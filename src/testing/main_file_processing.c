#include "../frontend/file_processing.h"
#include <stdio.h>

int main() {
    FileProcessing* test = createFileProcessing("../csv/matrix_multiplication_trace.csv");

    size_t numRequests;
    Request* requests;

    getRequests(test, &numRequests, &requests);

    for (size_t i = 0; i < numRequests; ++i) {
        printf("Request %zu: addr=0x%x, data=%u, we=%d\n", i, requests[i].addr, requests[i].data, requests[i].we);
    }

    free(requests);
    deleteFileProcessing(test);

    return 0;
}

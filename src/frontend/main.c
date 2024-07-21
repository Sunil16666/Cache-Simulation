#include <stdio.h>
#include <getopt.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

#include "file_processing.h"

struct Request
{
    uint32_t addr; ///< Memory address
    uint32_t data; ///< Requested Data
    int we; ///< WriteEnabled (true or false)
};

struct Result
{
    size_t cycles; ///< Number of cycles needed to complete the simulation
    size_t misses; ///< Number of total misses occured during the simulation
    size_t hits; ///< Number of total hits occured during the simulation
    size_t primitiveGateCount; ///< Number of primitive Gates needed to realize such Cache
};

extern struct Result run_simulation(
    int cycles,
    int directMapped,
    unsigned cacheLines,
    unsigned CacheLineSize,
    unsigned cacheLatency,
    unsigned memoryLatency,
    size_t num_Requests,
    struct Request* requests,
    const char* tracefile);


int toSanitizedInt(const char* optarg, int* result)
{
    char* endptr;
    long val;

    errno = 0; // Used to check if input is 0 and correct, or if the input is incorrect
    val = strtol(optarg, &endptr, 10);

    // Check for various possible errors
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
        || (errno != 0 && val == 0))
    {
#ifdef DEBUG
        perror("strtol");
#endif
        return -1;
    }


    if (endptr == optarg)
    {
#ifdef DEBUG
        fprintf(stderr, "No digits were found\n");
#endif
        return -1;
    }
    // Check for any trailing non-numeric characters
    if (*endptr != '\0')
    {
#ifdef DEBUG
        fprintf(stderr, "Non-numeric characters found: %s\n", endptr);
#endif
        return -1;
    }

    *result = (int)val;
    return 0;
}

int main(int argc, char* argv[])
{
    // Default values for simulation parameters
    int cycles = 1000;
    int directMapped = 0; //if directMapped & fullassociative are 0 the simulation will run fullassociative as default
    int fullassociative = 0;
    unsigned cacheLineSize = 8;
    unsigned cacheLines = 16;
    unsigned cacheLatency = 2;
    unsigned memoryLatency = 10;
    const char* tracefile = NULL;
    const char* input_file_path = "/csv/matrix_multiplication_trace.csv";

    static struct option long_options[] = {
        {"cycles", required_argument, 0, 'c'},
        {"directmapped", no_argument, 0, 'a'},
        {"fullassociative", no_argument, 0, 'b'},
        {"cacheline-size", required_argument, 0, 'd'},
        {"cachelines", required_argument, 0, 'e'},
        {"cache-latency", required_argument, 0, 'f'},
        {"memory-latency", required_argument, 0, 'g'},
        {"tf=", required_argument, 0, 'i'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int opt;
    int number_input;

    // Parameter handling
    while ((opt = getopt_long(argc, argv, "c:h", long_options, &option_index)) != -1)
    {
        switch (opt)
        {
        case 'a': //--directmapped
            {
#ifdef DEBUG
                printf("directmapped\n");
#endif
                directMapped = 1;
                break;
            }
        case 'b': //--fullassociative
            {
#ifdef DEBUG
                printf("fullassociative\n");
#endif

                fullassociative = 1;
                break;
            }
        case 'c': //--cycles <number> / -c <number>
            {
                if (toSanitizedInt(optarg, &number_input) == 0)
                {
#ifdef DEBUG
                    printf("cycles %d\n", number_input);
#endif

                    cycles = number_input;
                }
                else
                {
                    return 1;
                }
                break;
            }
        case 'd': //--cacheline-size <number>
            {
                if (toSanitizedInt(optarg, &number_input) == 0)
                {
#ifdef DEBUG
                    printf("cacheline-size %d\n", number_input);
#endif

                    cacheLineSize = number_input;
                }
                else
                {
                    return 1;
                }
                break;
            }
        case 'e': //--cachelines <number>
            {
                if (toSanitizedInt(optarg, &number_input) == 0)
                {
#ifdef DEBUG
                    printf("cachelines: %d\n", number_input);
#endif
                    cacheLines = number_input;
                }
                else
                {
                    return 1;
                }
                break;
            }
        case 'f': //--cache-latency <number>
            {
                if (toSanitizedInt(optarg, &number_input) == 0)
                {
#ifdef DEBUG
                    printf("cache-latency: %d\n", number_input);
#endif
                    cacheLatency = number_input;
                }
                else
                {
                    return 1;
                }
                break;
            }
        case 'g': //--memory-lateny <number>
            {
                if (toSanitizedInt(optarg, &number_input) == 0)
                {
#ifdef DEBUG
                    printf("memory-latency: %d\n", number_input);
#endif
                    memoryLatency = number_input;
                }
                else
                {
                    return 1;
                }
                break;
            }
        case 'h': //--help / -h
            {
                fprintf(stderr, "Usage: %s [options]\n", argv[0]);
                fprintf(stderr, "  -c, --cycles <number>      Set the number of cycles for the simulation\n");
                fprintf(stderr, "  --directmapped             Set cache mapping to direct mapped\n");
                fprintf(stderr, "  --fullassociative          Set cache mapping to fully associative\n");
                fprintf(stderr, "  --cacheline-size <size>    Set the cache line size\n");
                fprintf(stderr, "  --cachelines <number>      Set the number of cache lines\n");
                fprintf(stderr, "  --cache-latency <latency>  Set the cache latency\n");
                fprintf(stderr, "  --memory-latency <latency> Set the memory latency\n");
                fprintf(stderr, "  --tf=<filename>            Set the trace file name\n");
                fprintf(stderr, "  <filename>                 Positional Argument: Set the input file path\n");
                fprintf(stderr, "  -h, --help                 Display this help and exit\n");
                return 0;
            }
        case 'i': //--tf=<filename>
            {
                tracefile = optarg;
#ifdef DEBUG
                printf("Tracefile: %s\n", tracefile);
#endif
                break;
            }
        default:
            fprintf(stderr, "Unknown option: %s\n", argv[optind - 1]);
            fprintf(stderr, "Use -h or --help for displaying valid options.\n");
            return 1;
        }
    }

    if (fullassociative && directMapped)
    {
        perror("Please choose only one of --fullassociative or --directmapped");
        return 1;
    }

    struct Request* requests = NULL;
    size_t num_Requests = 0;

    //Positional parameter handling
    if (optind < argc)
    {
        input_file_path = argv[optind];

#ifdef DEBUG
        printf("Input file: %s\n", input_file_path);
#endif

        FileProcessing* fileProc = createFileProcessing(input_file_path);
        if (!fileProc)
        {
            fprintf(stderr, "Failed to initialize FileProcessing.\n");
            return 1;
        }

        getRequests(fileProc, &num_Requests, &requests);

        if (num_Requests > 0 && requests != NULL)
        {
#ifdef DEBUG
            printf("Fetched %zu requests:\n", num_Requests);
            for (size_t i = 0; i < num_Requests; i++)
            {
                printf("Request %zu: Addr = %u, Data = %u, WE = %d\n",
                       i, requests[i].addr, requests[i].data, requests[i].we);
            }
#endif
        }
        else
        {
            printf("No requests fetched or an error occurred.\n");
            return 1;
        }
        // Clean up
        deleteFileProcessing(fileProc);
    }

    const unsigned cacheSize = cacheLines * cacheLineSize;
    for (size_t i = 0; i < num_Requests; i++) // Check if all request adresses are within the bounds of the cache size
    {
        if (requests[i].addr > cacheSize)
        {
            fprintf(stderr, "Request %zu: Address %u is out of bounds for cache size %u\n", i, requests[i].addr,
                    cacheSize);
            free(requests);
            return 1;
        }
    }

    // Simulation
    struct Result result = run_simulation(
        cycles, directMapped, cacheLines, cacheLineSize,
        cacheLatency, memoryLatency, num_Requests,
        requests, tracefile
    );

    // Results
    printf("Simulation Results:\n");
    printf("Cycles: %zu\n", result.cycles);
    printf("Misses: %zu\n", result.misses);
    printf("Hits: %zu\n", result.hits);
    printf("Primitive Gate Count: %zu\n", result.primitiveGateCount);
    printf("Number of Requests: %zu\n", num_Requests);

    // print requests
    for (size_t i = 0; i < num_Requests; i++)
    {
        printf("Request %zu: Addr = %u, Data = %u, WE = %d\n",
               i, requests[i].addr, requests[i].data, requests[i].we);
    }

    free(requests);
    return 0;
}

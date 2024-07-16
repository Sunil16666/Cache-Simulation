#include <stdio.h>
#include <getopt.h>
#include <stdint.h>
#include <stdlib.h>


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


int main(int argc, char* argv[])
{
    // Default values for simulation parameters
    int cycles = 100;
    int directMapped = 1;
    int fullassociative = 0;
    unsigned cacheLineSize = 64;
    unsigned cacheLines = 128;
    unsigned cacheLatency = 2;
    unsigned memoryLatency = 10;
    const char* tracefile = "trace";

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

    while ((opt = getopt_long(argc, argv, "abc:d:e:f:g:i:h", long_options, &option_index)) != -1)
    {
        switch (opt)
        {
        case 'a': //--directmapped
            {
                printf("directmapped\n");
                directMapped = 1;
                break;
            }
        case 'b': //--fullassociative
            {
                printf("fullassociative\n");
                fullassociative = 1;
                break;
            }
        case 'c': //--cycles <number> / -c <number>
            {
                printf("cycles %s\n", optarg);
                cycles = atoi(optarg);
                break;
            }
        case 'd': //--cacheline-size <number>
            {
                printf("cacheline-size %s", optarg);
                cacheLineSize = atoi(optarg);
                break;
            }
        case 'e': //--cachelines <number>
            {
                cacheLines = atoi(optarg);
                break;
            }
        case 'f': //--cache-latency <number>
            {
                cacheLatency = atoi(optarg);
                break;
            }
        case 'g': //--memory-lateny <number>
            {
                memoryLatency = atoi(optarg);
                break;
            }
        case 'h': //--help / -h
            {
                fprintf(stderr, "Usage: %s [options]\n", argv[0]);
                fprintf(stderr, "  -c, --cycles <number>       Set the number of cycles for the simulation\n");
                fprintf(stderr, "  -a, --directmapped          Set cache mapping to direct mapped\n");
                fprintf(stderr, "  -b, --fullassociative       Set cache mapping to fully associative\n");
                fprintf(stderr, "  -d, --cacheline-size <size> Set the cache line size\n");
                fprintf(stderr, "  -e, --cachelines <number>   Set the number of cache lines\n");
                fprintf(stderr, "  -f, --cache-latency <latency> Set the cache latency\n");
                fprintf(stderr, "  -g, --memory-latency <latency> Set the memory latency\n");
                fprintf(stderr, "  -i, --tf=<filename>         Set the trace file name\n");
                fprintf(stderr, "  -h, --help                  Display this help and exit\n");
                return 0;
            }
        case 'i': //--tf=<filename>
            {
                tracefile = optarg;
                break;
            }
        default:
            fprintf(stderr, "Unknown option: %s\n", argv[optind - 1]);
            fprintf(stderr, "Use -h or --help for displaying valid options.\n");
            return 1;
        }
    }


    // Example memory requests
    struct Request requests[] = {
        {0x0040, 0x1, 1},
        {0x00A0, 0x2, 0}
    };
    size_t num_Requests = sizeof(requests) / sizeof(requests[0]);

    // Run the simulation
    struct Result result = run_simulation(
        cycles, directMapped, cacheLines, cacheLineSize,
        cacheLatency, memoryLatency, num_Requests,
        requests, tracefile
    );

    // Output results
    printf("Simulation Results:\n");
    printf("Cycles: %zu\n", result.cycles);
    printf("Misses: %zu\n", result.misses);
    printf("Hits: %zu\n", result.hits);
    printf("Primitive Gate Count: %zu\n", result.primitiveGateCount);

    return 0;
}

#include <stdio.h>
#include <getopt.h>

int main(int argc, char *argv[])
{
    static struct option long_options[] = {
        {"cycles", required_argument, 0, 'a'},
        {"directmapped", no_argument, 0, 'b'},
        {"fullassociative", no_argument, 0, 'c'},
        {0, 0, 0, 0}
    };

}
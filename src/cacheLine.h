#ifndef CACHELINE_H
#define CACHELINE_H

/**
 * Structure representing a cache line
 */
struct CacheLine
{
    uint32_t* data; ///< Data stored in the cache line
    bool* valid;    ///< Valid flag for each data element
    uint32_t tag;   ///< Tag of the cache line

    /**
     * explicit Constructor, no implicit conversion allowed
     */
    explicit CacheLine(unsigned size)
    {
        // Allocate memory for the data and valid arrays
        data = new uint32_t[size];
        valid = new bool[size];
        tag = 0;

        // Default valid-flag is false
        for (int i = 0; i < size; i++)
        {
            valid[i] = false;
        }
    }

    /**
     * Destructor
     */
    ~CacheLine()
    {
        delete[] data;
        delete[] valid;
    }
};

#endif //CACHELINE_H
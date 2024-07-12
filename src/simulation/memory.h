#ifndef MEMORY_H
#define MEMORY_H
#include <map>
#include <systemc.h>

/**
 * Memory Module for the Simulation
 */
class Memory : public sc_module
{
public:
    sc_in<bool> clk;        ///< Clock Signal
    sc_in<bool> we;         ///< Write Enable Signal
    sc_in<uint32_t> addr;   ///< Address Signal
    sc_in<uint32_t> wdata;  ///< Write Data Signal

    sc_out<uint32_t> rdata; ///< Read Data Signal

    SC_HAS_PROCESS(Memory); ///< Macro for multiple-argument constructor of the Module

    /**
     * Constructor of the Module
     * @param name
     */
    Memory(sc_module_name name) : sc_module(name)
    {
        // Defining the process of the Module
        SC_THREAD(process);
        sensitive << clk.pos() << we << addr << wdata;
    }

    // Clean up the memory
    void initialize()
    {
        memory.clear();
    }

private:
    std::map<uint32_t, uint32_t> memory;     ///< Memory Map (Address, Data)

    void write(uint32_t addr, uint32_t data) ///< Write to the Memory
    {
        memory[addr] = data;
    }

    void read(uint32_t addr)                 ///< Read from the Memory
    {
        auto it = memory.find(addr);         ///< Find the address in the memory
        if (it != memory.end())              ///< If found
        {
            rdata.write(it->second);         ///< Write the data to the read data signal
        }
        else
        {
            rdata.write(0);            ///< Else write 0
        }
    }

    void process()                                      ///< Process the memory requests
    {
        if (we.read())                                  ///< If write enabled
        {
            write(addr.read(), wdata.read());  ///< Write to memory
        }
        else
        {
            read(addr.read());                     ///< Else read from memory
        }
    }
};


#endif //MEMORY_H
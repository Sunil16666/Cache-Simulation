#ifndef MEMORY_H
#define MEMORY_H
#include <map>
#include <systemc.h>

class Memory : public sc_module
{
public:
    sc_in<bool> clk;
    sc_in<bool> we;
    sc_in<uint32_t> addr;
    sc_in<uint32_t> wdata;
    sc_out<uint32_t> rdata;

    SC_HAS_PROCESS(Memory);

    Memory(sc_module_name name) : sc_module(name)
    {
        SC_METHOD(process);
        sensitive << clk.pos();
        initialize();
    }

    void initialize()
    {
        memory.clear();
    }

private:
    std::map<uint32_t, uint32_t> memory;

    void write(uint32_t addr, uint32_t data)
    {
        memory[addr] = data;
    }

    void read(uint32_t addr)
    {
        rdata.write(memory[addr]);
    }

    void process()
    {
        if (we.read())
        {
            write(addr.read(), wdata.read());
        }
        else
        {
            read(addr.read());
        }
    }
};


#endif //MEMORY_H

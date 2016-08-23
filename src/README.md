# README


## hello

This is just a basic "Hello, world!" program in classic SystemC.


## cpu_ram

This is an example of a loosely-timed TLM program. A CPU reads instructions from an assembly file and make memory accesses to a RAM.


### Master module

All initiator methods and fields are inherited from the `Master` module. This module provides its children with RAM read and write methods, implemented using transactions. TLM-specific code is thus encapsulated into this module, allowing the `CPU` module to interact with the RAM seemlessly.

As it is the duty of the initiator to keep track of time, `Master` possesses a quantum keeper field that advances the local time when the targets answer back with a delay. This also helps providing a `void sleep(sc_time)` method to the child classes.

The only requirement when implementing this abstract class is to define a `void process(void)`method that will get started as a SystemC thread.


### CPU module

The CPU's main process just reads the provided file line by line and interprets it as an instruction. For that purpose, the CPU module holds a set of registers which can be loaded from or stored to RAM (they also benefit from a debug `dmp` instruction to print them on `stdout`).

The CPU models the time necessary to execute an instruction by incrementing the local time offset (using Master's sleep method).


### Memory module

Following the same pattern, the `Memory` module encapsulates TLM's mandatory implementation. It only asks child classes to implement read and write methods and some getters .
It supports debug transport, byte enables, and DMI but does not support streaming. It also features a dumping-to-stdout debug method to inspect the memory content.


### RAM module

A classic memory model implemented with arrays and `memcpy` calls.


### Instruction file

The instruction file supports comments (with a leading '#') and the following commands:
- `nop`: does nothing but still consumes time because of the instruction latency;
- `dmp`: dumps all the registers' content to `stdout`;
- `chr`: puts a constant value in a register;
- `ldr`: loads a value from a memory address to a register;
- `str`: stores a value from a register to a memory address;
- `add`: adds the content of two registers into a third;
- `sub`: substracts the content of two registers into a third one;
- `mul`: multiplies the content of two registers into a third one;
- `div`: divides the content of two registers into a third one.


## multicpu_multimem

This example shows CPUs accessing a RAM and some GPIOs that are mapped to an interconnect bus.


### Differences with cpu_ram

The CPU and Master modules were pretty much left intouched, except for the speed and memory fix that consists in always using the same payload object instead of instanciating a new one each time.

The RAM module stays the same, the Memory too except for the DMI support that now falls to the implementing class' choosing.


### GPIO module

Even though each GPIO pin is mapped as a whole byte on the interconnect, it is stored as a bit in memory, making the module implementation different than the RAM's which trivially calls `memcpy`.


### Bus module

Because the interconnect bus is wired to several masters and slaves, it makes us of `multi_passthrough_<initiator|target>_socket` objects instead of `simple_<initiator|targets>_socket`. This allows several initiators or targets to be connected to the same socket.

The role of the bus is to transfer the TLM protocol's calls to the right master or slave. For that purpose, the module performs address translation, converting a slave id and a real address to a virtual (mapped) address, and vice versa.

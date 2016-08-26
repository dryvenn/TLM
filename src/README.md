# TLM 2.0 implementation

## Examples

In this folder are samples of TLM 2.0 implementations.


### hello

This is just a basic "Hello, world!" program in classic SystemC.


### cpu_ram

This is an example of a loosely-timed TLM program. A CPU reads instructions from an assembly file and make memory accesses to a RAM.


#### Master module

All initiator methods and fields are inherited from the `Master` module. This module provides its children with RAM read and write methods, implemented using transactions. TLM-specific code is thus encapsulated into this module, allowing the `CPU` module to interact with the RAM seemlessly.

As it is the duty of the initiator to keep track of time, `Master` possesses a quantum keeper field that advances the local time when the targets answer back with a delay. This also helps providing a `void sleep(sc_time)` method to the child classes.

The only requirement when implementing this abstract class is to define a `void process(void)`method that will get started as a SystemC thread.


#### CPU module

The CPU's main process just reads the provided file line by line and interprets it as an instruction. For that purpose, the CPU module holds a set of registers which can be loaded from or stored to RAM (they also benefit from a debug `dmp` instruction to print them on `stdout`).

The CPU models the time necessary to execute an instruction by incrementing the local time offset (using Master's sleep method).


#### Memory module

Following the same pattern, the `Memory` module encapsulates TLM's mandatory implementation. It only asks child classes to implement read and write methods and some getters .
It supports debug transport, byte enables, and DMI but does not support streaming. It also features a dumping-to-stdout debug method to inspect the memory content.


#### RAM module

A classic memory model implemented with arrays and `memcpy` calls.


#### Instruction file

The instruction file supports comments (with a leading '##') and the following commands:
- `nop`: does nothing but still consumes time because of the instruction latency;
- `dmp`: dumps all the registers' content to `stdout`;
- `chr`: puts a constant value in a register;
- `ldr`: loads a value from a memory address to a register;
- `str`: stores a value from a register to a memory address;
- `add`: adds the content of two registers into a third;
- `sub`: substracts the content of two registers into a third one;
- `mul`: multiplies the content of two registers into a third one;
- `div`: divides the content of two registers into a third one.


### multicpu_multimem

This example shows CPUs accessing a RAM and some GPIOs that are mapped to an interconnect bus.


#### Differences with cpu_ram

The CPU and Master modules were pretty much left intouched, except for the speed and memory fix that consists in always using the same payload object instead of instanciating a new one each time.

The RAM module stays the same, the Memory too except for the DMI support that now falls to the implementing class' choosing.


#### GPIO module

Even though each GPIO pin is mapped as a whole byte on the interconnect, it is stored as a bit in memory, making the module implementation different than the RAM's which trivially calls `memcpy`.


#### Bus module

Because the interconnect bus is wired to several masters and slaves, it makes us of `multi_passthrough_<initiator|target>_socket` objects instead of `simple_<initiator|targets>_socket`. This allows several initiators or targets to be connected to the same socket.

The role of the bus is to transfer the TLM protocol's calls to the right master or slave. For that purpose, the module performs address translation, converting a slave id and a real address to a virtual (mapped) address, and vice versa.


## Architecture comparison

Let us now compare the design of these example with ARM's extension of TLM 2.0 for the AMBA buses.

![AMBA-PV architecture](https://github.com/dryvenn/TLM/blob/master/res/amba_arch.png)
*Diagram of the AMBA-PV architecture*


![cpu_ram architecture](https://github.com/dryvenn/TLM/blob/master/res/my_arch.png)
*Diagram of the cpu_ram  architecture*


These two architectures are very similar in their approach: let the main classes focus on the fonctionnality and delegate the protocol to generic ones. This is essentially the pattern recommended by OSCI's TLM: embed behavioral models into communication wrappers.

The `master` and `CPU` class both contain the threaded function that will drive the model. To communicate with the targets, they use their parent class methods. The `slave` and `RAM` receive the transactions through the methods of their parent class, and fulfil the request.

However, the two implementations differ when it comes to sockets:

Firstly, cpu_ram uses the generic utilities of TLM (the simple sockets), whereas AMBA_PV has custom sockets (though based on the simple ones). This allow the protocol to be more complex, for example to support exclusive reads and writes.

Secondly, in the cpu_ram design, the base classes contain the socket objects, whereas in the AMBA_PV design, sockets are possessed by the extending classes. In practice, this means that instead of bothering itself to make up a transaction object and call the `b_transport` method, the `CPU` class just invokes `read` or `write`, letting to its parent class to take care of the rest. However, the AMBA-PV implementation does not do that either; instead on calling the socket's `b_transport` method, the extending class calls its `read` and `write` methods, which are part of the custom protocol. Even though this solution lacks simplicity because several objects have to be built and passed on for each transaction, it gives more control to the caller which can fine-tune every aspect of the extended protocol.

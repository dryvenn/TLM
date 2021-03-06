# Transaction Level Modeling in SystemC


## What is TLM?


### Introduction

The idea behind SystemC is to offer several levels of hardware abstraction in a unique and ubiquitous language: C++. Each of these levels constitutes a different **model** of the system, meaning that it is a way of representing it.

The most well-known level of abstraction is the Register Transfer Level (RTL). Languages such as Verilog and VHDL abstract low-level things like wiring and gates to focus on the logic. Processes are sensitive to the pulses of common signals such as the main clock, which guarantees exact synchronisation.

Another way of modeling the system is to consider the transactions between its components. At a higher level than the RTL, the Transaction Level (TL) abstracts both the elements of the system, which become mere behavioral models embedded into wrappers, and the communication between them. This means that processes no longer have a clock or other signals to synchronise to, they just exchange transactions and consume simulation time.


### Use cases

As this high-level model of the chip runs faster than RTL (between 100 and 100,000 times faster), it empowers engineers in the following fields.


#### Software development

As the TLM is more coarse-grained than the RTLM, needing only the behavioral models of the IPs and not their detailed implementation, the model can be developed in a much smaller time. And once it is effectively complete, software developers can begin their work even before RTL is ready.


#### Software performance

Software developers often find themselves in the need of testing programs by measuring their speed. However, simulating RTL often takes a very long time, leading the tests to be scarce, thus inefficient. By being way faster than RTL, TL modeling makes tests cheaper.


#### Architecture analysis

Because TL comes before RTL, it can be used to validate the architectural choices of the chip; and possibly correct them even before RTL is coded.


#### Hardware verification

As TLM reflects the true behavior of the chip, it calls for design inspection so that bugs can be detected and fixed at an early stage.


### Constraints

After having gone through the potential use cases of TLM, the constraints specific to this level of modeling appear brightly.

Firstly, TLM must be **fast**. There is no point in not developing and using RTL directly if it is not to gain a decisive speed advantage.

Secondly, TLM must be **early**. Indeed, RTL must be a more fine-grained version of TL, therefore TL must be first. Furthermore, the earlier TLM is available, the earlier software developers can begin to work.

Thirdly, TLM must be **accurate**. This is the only way to get meaningful results when assessing software performance and architecture quality.


## SystemC implementation


### Overview

TLM 2.0 was released in June 2008, and is compatible with SystemC 2.2.0 and higher. The core concept of its implementation is to replace any communication between IPs by a single function call. This call -replacing pins wiggling- will cary the transaction object and therefore all its characteristics. As for the IPs themselves, only a behavioral model in C++ is expected, which will be embedded into a communication wrapper to handle the transactions.


![TLM's implementation overview](https://github.com/dryvenn/TLM/blob/master/res/overview.png)


With the version 2.0 of TLM, the goal of the Open SystemC Initiative (OSCI) is to promote interoperability between components.
It does so by supplying developers with 3 things:
- a standard set of APIs;
- generic utility classes;
- coding style directives.
This aim to enhance the productivy of developers and the synergy of their communities.


### Main concepts

The `transaction` objects which contain data in form of a `payload` are passed from one component to another through `sockets`. An `initiator` thus makes a request to a `target`, and will in some cases get a response.
Generic classes for payloads and sockets are built-in.

![TLM's main concepts](https://github.com/dryvenn/TLM/blob/master/res/main_concepts.png)


#### Targets

A target's job is to receive a request from the initiator, make up a response based on the IP's behavioral model, and send it back. The most basic target is a SystemC module that contains TLM's target socket, namely `tlm::tlm_target_socket`. This class can be extended to obtain more elaborate targets and protocols while preserving interoperability. TLM also provides the convenience of a simple target socket to help developers.

```C++
SC_MODULE(Target) {
	/* Simple target socket
	 *
	 * Parameters:
	 *	- module name (mandatory);
	 *	- bus width (optional, defaults to 32);
	 *	- protocol types, which mainly concerns the type of payload (optional,
	 *	  defaults to `tlm_base_protocol_types').
	 */
	tlm_utils::simple_target_socket<Target, 32, tlm::tlm_base_protocol_types> socket;

	SC_CTOR(Target) : socket("socket") {
		...
	}

	...

};
```


#### Initiators

An initiator's job is to send requests to the target based on the IP's behavioral model, and receive the responses. Just like targets, the most basic initiator is a SystemC module that contains TLM's initiator socket, namely `tlm::tlm_initiator_socket`, which can also be extended and has been for its simple version.

```C++
SC_MODULE(Initiator) {
	tlm_utils::simple_initiator_socket<Initiator, 32, tlm::tlm_base_protocol_types> socket;

	SC_CTOR(Initiator) : socket("socket") {
		...
	}

	...

};
```


#### Transactions

The most basic transaction is made up of an instance of `tlm::tlm_generic_payload`, which has the following fields:
- `command`: Whether it's a read or a write.
- `address`: The base address of the transaction.
- `data`: A pointer to a buffer to write data to or read data from.
- `length`: Size of the data in byte.
- `response_status`: The status of the target's response (it might be an error). This field must be set to `TLM_INCOMPLETE_RESPONSE` by the initiator.
- `byte_enable` and `byte_enable_length`: To use byte enables.
- `streaming_width`: A value greater than 0 and lesser than `length` allows streaming bursts.
- `dmi_allowed`: This field must be set to false by the initiator. If the target sets it to true, it means that Direct Memory Interface (DMI) is available.
- `extensions`: It is possible to extend the transaction by adding fields to the payload in order to match a more elaborate protocol. In that case, this field will hold instance of the `tlm::tlm_extension<T>` class.


#### Blocking transport

Once the generic payload is populated, it only needs to be transmitted from the initiator to the target. For that purpose the target implements a transport function so that it is available to the initiator.

Using `void b_transport(tlm::tlm_generic_payload&, sc_time&)` is the easiest way of doing it. It is a blocking version of the protocol, meaning that the initiator regains control only when the target has finished using the payload and modifying it accordingly. Indeed, the target's response is in the modified generic payload; the return path has been used to transmit it.

There is also a time parameter in the function's prototype. It is supposed to be incremented by the target to give an estimate of the real computation time. It is to the initiator to choose whether to take it into account.

We now see that the initiator is responsible for two things:
- memory management, for the generic payload and the buffers (and it can be optimized, for example by using a pool of transactions);
- time management, so that the model stays accurate.


#### Direct Memory Interface

Within the generic payload, there is one interesting field for optimization: the Direct Memory Interface. Indeed, it is not efficient at all to constantly fill up and pass on a transaction object just to access the same segment of memory. For this reason, TLM provides a mechanism to bypass the standard protocol and direcly access memory.

After a successful transaction, a target might have set the `dmi_allowed` flag, meaning that the feature is available. The initiator can then call a function implemented by the target: `bool get_direct_mem_ptr(tlm::tlm_generic_payload&, tlm::tlm_dmi&)` to get a pointer to the memory. The `tlm_dmi` structure contains information about the requested memory segment: start, end, read and write permissions, read and write latencies.

With this pointer, the initiator can tremendously speed up its accesses, while keeping track of the latency-induced delays which serves the model's accuracy constraint.

Giving away access to its memory is not irreversible for the target. The initiator must also implement a function: `void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64)`. Calling it invalidates the DMI pointer for a range of addresses.

We now see that the target is responsible for two things:
- operation management, to fulfil the initiator's request;
- DMI management, whether it has become available or not.


### Coding styles

Depending on the use case, it might be better sometimes to aim for a greater simulation speed (for instance when running software on top of TLM), and some other times to get a greater accuracy (for instance when doing hardware verification). TLM provides two coding styles to do just that: the Loosely-Timed (LT) and Approximately-Timed (AT) styles.


#### Loosely-Timed

In TLM the IP models run concurrently but on the same simulation machine, so they have to take turns in order to progress. Normally, when the simulation is Cycle-Accurate and Bit-Accurate (CABA), processes (SystemC's threads and methods) run for one cycle at each turn. And even though this yields a huge accuracy, the overhead of each context switch lead to a massive waste of time. LT is about letting each process run as far as it can (usually stopping when it needs to communicate), which minimizes the number of context switches. This is called **temporal decoupling**.

However, there should still be a notion of turns so that one process does not monopolize processor time. For this reason, a process cannot run longer than a pre-defined **quantum** of time. This quantum thus defines the granularity of the simulation: a large quantum means a big speedup but also a loss in timing accuracy.


##### Keeping track of time

Temporal decoupling in TLM relies on different time variables to achieve this "loose" synchronisation:
- (System) Global Quantum: This is the quantum mentionned above, the running time each process gets before synchronisation. It should be set once at the beginning of the simulation within the singleton class `tlm::tlm_global_quantum`.
- (Thread) Global Quantum: This is a per-thread record of the Global Quantum. TLM provides threads with a utility to hold quantum-related variables: `tlm_utils::tlm_quantumkeeper`. Best practices recommend to permanently set the Thread Global Quantum to the same value as the System Global Quantum in the thread's constructor.
- Local Quantum: This is the remaining time in the current quantum, namely the difference between the current Thread Global Quantum's end and the current simulation time (obtained with `sc_time_stamp`). Because the System Quantum should the same as the Thread Quantum, it should always be the same value for all threads.
- Local Time Offset: This is the difference between the thread's own time and the current simulation time. Indeed, even though threads cannot run longer than the quantum, they can run shorter. After an operation, a thread can advance the model time, which is the current simulation time plus the local time offset; and if it becomes greater than the quantum, then the thread needs synchronisation. The quantum keeper has a `need_sync` and a `sync` method for that.


#### Approximately-Timed

Even though blocking transports and return paths make up a simple and efficient implementation, it does not quite reflect the way hardware and protocol work in a real chip. The AT style allows for a better accuracy by breaking down the transactions into phases that may better represent the protocol's timings.

Transactions are thus split into requests and responses, themselves split into beginnings and ends. This results in four generic phases which can be extended to match more elaborate protocols.
![TLM AT transactions](https://github.com/dryvenn/TLM/blob/master/res/AT.png)

To do that, TLM provides non-blocking functions that work in both ways: `nb_transport_fw` which takes the forward path (from the initiator to the target), and `nb_transport_bw` which takes the backward path (from the target to the initiator). They also take the transaction object and the delay as arguments, added to the current phase. It is also possible for processes to skip phases by using an error code on the return path instead of answering with a function call. However, TLM has set a limitation regarding the phases known as the *exclusion rule*: there cannot be more than one request or more than one response in progress in a socket.


##### Keeping track of time

The AT style, to achieve its goal of being just accurate enough, needs to consume simulation time throughout the model's phases. TLM also provides utilities for this purpose, the payload event queues. For instance, an initiator can instantiate a `tlm_utils::peq_with_cb_and_phase` with a callback parameter and use its `notify` method when receiving a response, passing on the transaction, the phase, and the delay. Once the delay has passed, the callback is invoked with the transaction and the phase as arguments, allowing the thread to process them at the right time.

For this system to work, threads have to play nice with each other by periodically calling the `wait` function, especially when it comes to honor the exclusion rule by waiting until the previous request or response has finished.


### An example of protocol extension

TLM has been applied to ARM's Advanced Microcontroller Bus Architecture (AMBA). But as its protocol is far too complicated for the generic classes of TLM 2.0, some modifications had to be made. For instance, the bus model needs to support information like "Secure/Non-secure/Privileged", and there is no mechanism for that in the base protocol. So, to preserve interoperability and respect TLM 2.0's design guidelines, they did the following:
- Generic classes such as simple sockets were extended to support the features' management;
- Extensions were added to the generic payload so that the additional information can be conveyed;
- The classic blocking transport function was used, thus respecting the Loosely-Timed coding style.

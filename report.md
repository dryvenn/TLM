# Transaction Level Modeling in SystemC


## What is TLM?


### Introduction

The idea behind SystemC is to offer several levels of hardware abstraction in a unique and ubiquitous language: C++. Each of these levels constitutes a different **model** of the system, meaning that it is a way of representing it.

The most well-known level of abstraction is the Register Transfer Level (RTL). Languages such as Verilog and VHDL abstract low-level things like wiring and gates to focus on the logic.

Another way of modeling the system is to consider the transactions between its components. At a higher level than the RTL, the Transaction Level (TL) abstracts both the elements of the system, which become mere behavioral models embedded into wrappers, and the communication between them.


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

The most basic target is a SystemC module that contains TLM's built-in target socket. The job of the target is to receive the request of the initiator, make up a response based on the IP's behavioral model, and send it back.

```C++
SC_MODULE(Target) {
	tlm_utils::simple_target_socket<Target> socket;

	SC_CTOR(Target) : socket("socket") {
		...
	};

	...

};
```


#### Initiators

The most basic initiator is a SystemC module that contains TLM's built-in initiator socket. The job of the initiator is to send requests to the target based on the IP's behavioral model, and receive the responses.

```C++
SC_MODULE(Initiator) {
	tlm_utils::simple_initiator_socket<Initiator> socket;

	SC_CTOR(Initiator) : socket("socket") {
		...
	};

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

In TLM the IP models run concurrently but on the same simulation machine, so they have to take turns in order to progress. Normally, when the simulation is Cycle-Accurate and Bit-Accurate (CABA), processes (SystemC's threads and methods) run for one cycle at each turn. And even though this yields a huge accuracy, the overhead of each context switch lead to a massive waste of time. The idea behind the LT style is to let each process run as far as it can (usually stopping when it needs to communicate), which minimizes the number of context switches. This is called **temporal decoupling**.

However, there should still be a notion of turns so that one process does not monopolize processor time. For this reason, a process cannot run longer than a pre-defined **quantum** of time. This quantum thus defines the granularity of the simulation: a large quantum means a big speedup but also a loss in timing accuracy.


#### Approximately-Timed

Getting a better timing accuracy is made possible by the AT style. In this style, not only transactions include timing annotations but also indications on the current phase (begin/end of request, begin/end of response). Processes, in synchronisation with the simulation time, use `wait` and `notify` function calls to consume it.

However, there can be no blocking transport anymore. In the LT style, the `b_transport` function was particularely appropriate because the initiator would hand control over to the target only for the time necessary to serve the request. But this could lead to timing inconsistencies because there is not any control over the elapsed time. So instead of using one blocking transport function, the AT style uses two non-blocking functions: `nb_transport_fw` which takes the forward path (from the initiator to the target), and `nb_transport_bw` which takes the backward path (from the target to the initiator). And it is to phases to keep up with the state of the transaction. This way, requests and responses can be handled without compromising the model's accuracy.


### An example of protocol extension

TLM has been applied to ARM's Advanced Microcontroller Bus Architecture (AMBA). But as its protocol is far too complicated for the generic classes of TLM 2.0, some modifications had to be made. For instance, the bus model needs to support information like "Secure/Non-secure/Privileged", and there is no mechanism for that in the base protocol. So, to preserve interoperability and respect TLM 2.0's design guidelines, they did the following:
- Generic classes such as simple sockets were extended to support the features' management;
- Extensions were added to the generic payload so that the additional information can be conveyed;
- The classic blocking transport function was used, thus respecting the Loosely-Timed coding style.

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

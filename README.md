CoDi
===

CoDi is a cellular automaton (CA) model for spiking neural networks (SNNs). CoDi is an acronym for Collect and Distribute, referring to the signals and spikes in a neural network.

CoDi uses a von Neumann neighborhood modified for a three-dimensional space; each cell looks at the states of its six orthogonal neighbors and its own state. In a growth phase a neural network is grown in the CA-space based on an underlying chromosome. There are four types of cells: neuron body, axon, dendrite and blank. The growth phase is followed by a signaling- or processing-phase. Signals are distributed from the neuron bodies via their axon tree and collected from connection dendrites. These two basic interactions cover every case, and they can be expressed simply, using a small number of rules.

Install
===

You need a c++11 compiler, and SFML 2 installed. 

Then just run `make` and then `./codi` which will compile and start the program.

TODO
===

Need learning & training algorithm. 

---> Lernverfahren für evolutionär optimierte Künstliche Neuronale Netze auf der Basis Zellulärer Automaten

I don't know German :[
#ifndef CELL_TYPE_H
#define CELL_TYPE_H

enum cell_type {
	BLANK           = 0,
	NEURONSEED      = 1,
	NEURON          = 1,
	AXON            = 2,
	AXON_SIGNAL     = 2,
	DENDRITE        = 4,
	DENDRITE_SIGNAL = 4,
};

#endif
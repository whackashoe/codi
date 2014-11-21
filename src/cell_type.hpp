#ifndef CELL_TYPE_H
#define CELL_TYPE_H

#include <cstdint>

/* We do not use explicit synapses, 
 * because dendrite cells that are in contact with an axonal trail 
 * (i.e. have an axon cell as neighbor) collect the neural signals directly from the axonal trail. 
 * This results from the behavior of axon cells, which distribute to every neighbor, 
 * and from the behavior of the dendrite cells, which collect from any neighbor.
 */
enum cell_type : std::uint8_t {
    BLANK           = 0, //empty cell
    NEURONSEED      = 1, //prerequisite for neuron growth
    NEURON          = 1, //The neuron body cells collect neural signals from the surrounding dendritic cells 
    AXON            = 2, //Axonal cells distribute data originating from the neuron body.
    AXON_SIGNAL     = 2, //
    DENDRITE        = 4, //Dendritic cells collect data and eventually pass it to the neuron body.
    DENDRITE_SIGNAL = 4,
};

#endif
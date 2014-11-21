#ifndef NETWORK_H
#define NETWORK_H

#include <array>
#include <bitset>
#include <numeric>
#include <random>
#include <cstdint>
#include <iostream>
#include "config.hpp"
#include "cell.hpp"
#include "cell_type.hpp"
#include "utility.hpp"



/* CoDi is a cellular automaton (CA) model for spiking neural networks (SNNs). 
 * CoDi is an acronym for Collect and Distribute, 
 * referring to the signals and spikes in a neural network.
 * 
 * CoDi uses a von Neumann neighborhood modified for a three-dimensional space
 * each cell looks at the states of its six orthogonal neighbors(EWNSUD) and its own state. 
 * Signals are distributed from the neuron bodies via their axon tree and collected from connection dendrites.
 * These two basic interactions cover every case, and they can be expressed simply, using a small number of rules.
 */
template <int GSize>
class network
{
private:
    bool changed;
    bool has_setup_signaling;


    /* For the Neighborhood interaction.
     * Names for the buffer correspond to the I-Buf,
     * so where the Information (signal) came from;
     * e.g. in the north buffer (+y=2) of a cell is either what came
     * from the north (after kick) or the what will go to the south 
     * (before kick).
     * iobuf[0..5] = east(+x), west(-x), north(+y), south(-y),
     * top(+z), bottom(-z) 
     * actualize the 3 direction one at a time, without wrap-around.
     */
    void kicking()
    {

        // For the positive directions
        for(int iz=0; iz<GSize; ++iz) {
            for(int iy=0; iy<GSize; ++iy) {
                for(int ix=0; ix<GSize; ++ix) {
                    grid[iz][iy][ix].iobuf[4] = (iz != GSize-1) ? grid[iz+1][iy][ix].iobuf[4] : 0;
                    grid[iz][iy][ix].iobuf[2] = (iy != GSize-1) ? grid[iz][iy+1][ix].iobuf[2] : 0;
                    grid[iz][iy][ix].iobuf[0] = (ix != GSize-1) ? grid[iz][iy][ix+1].iobuf[0] : 0;
                }
            }
        }

        // For the negative directions
        for(int iz=GSize-1; iz >= 0; --iz) {
              for(int iy=GSize-1; iy >= 0; --iy) {
                for(int ix=GSize-1; ix >= 0; --ix) {
                    grid[iz][iy][ix].iobuf[5] = (iz != 0) ? grid[iz-1][iy][ix].iobuf[5] : 0;
                    grid[iz][iy][ix].iobuf[3] = (iy != 0) ? grid[iz][iy-1][ix].iobuf[3] : 0;
                    grid[iz][iy][ix].iobuf[1] = (ix != 0) ? grid[iz][iy][ix-1].iobuf[1] : 0;
                }
            }
        }
    }


    void setup_signaling()
    {
        std::uniform_int_distribution<int> three_two_rng(0, 32);
        has_setup_signaling = true;
        
        for(int iz=0; iz<GSize; ++iz) {
            for(int iy=0; iy<GSize; ++iy) {
                for(int ix=0; ix<GSize; ++ix) {
                    grid[iz][iy][ix].activation = 0;
                    grid[iz][iy][ix].iobuf.fill(0);

                    if(grid[iz][iy][ix].type == NEURON) {
                        grid[iz][iy][ix].activation = three_two_rng(rng_gen);
                    }
                }
            }
        }
    }


    /* In a growth phase a neural network is grown in the CA-space based on an underlying chromosome. 
     * The growth phase is followed by a signaling- or processing-phase
     */
    void growth_step()
    {
        int input_sum { 0 };
        changed = false;
        
        for(int iz=0; iz<GSize; ++iz) {
            for(int iy=0; iy<GSize; ++iy) {
                for(int ix=0; ix<GSize; ++ix) {
                    switch(grid[iz][iy][ix].type) {
                        case BLANK: 
                            if(grid[iz][iy][ix].is_neuronseed()) {
                                changed = true;
                                grid[iz][iy][ix].type = NEURON;
                                // inform the neighbors immediately 
                                grid[iz][iy][ix].gate = (grid[iz][iy][ix].chromo & 63) % 6;
                                grid[iz][iy][ix].iobuf.fill(DENDRITE_SIGNAL);
                                grid[iz][iy][ix].iobuf_from_gate() = AXON_SIGNAL;
                                grid[iz][iy][ix].iobuf_from_adjacent_gate() = AXON_SIGNAL;
                                break;
                            }

                            /* The blank neighbors, which receive a neural growth signal, turn into either an axon cell or a dendrite cell.
                             * The growth signals include information containing the cell type of the cell that is to be grown from the signal. 
                             * To decide in which directions axonal or dendritic trails should grow, 
                             * the grown cells consult their chromosome information which encodes the growth instructions. 
                             * These growth instructions can have an absolute or a relative directional encoding. 
                             * An absolute encoding masks the six neighbors (i.e. directions) of a 3D cell with six bits.
                             * After a cell is grown, it accepts growth signals only from the direction from which it received its first signal. 
                             * This reception direction information is stored in the gate position of each cell's state.
                             */

                            // test for no signal 
                            input_sum = fold_plus(grid[iz][iy][ix].iobuf);
                            if(input_sum == 0) {
                                break;
                            }

                            // test for axon signals
                            input_sum = fold_plus_and(grid[iz][iy][ix].iobuf, AXON_SIGNAL); 
                            if(input_sum == AXON_SIGNAL) {
                                changed = true;
                                grid[iz][iy][ix].type = AXON;

                                for(int i=0; i<6; ++i) {
                                    if(grid[iz][iy][ix].iobuf[i] == AXON) {
                                        grid[iz][iy][ix].gate = i;
                                    }

                                    grid[iz][iy][ix].iobuf[i] = grid[iz][iy][ix].chromo_dir_choice(i, AXON_SIGNAL, 0);
                                }
                                break;
                            }

                            if(input_sum > AXON_SIGNAL) {
                                grid[iz][iy][ix].iobuf.fill(0);
                                break;
                            }

                            // test for dendrite signals
                            input_sum = fold_plus_and(grid[iz][iy][ix].iobuf, DENDRITE_SIGNAL);
                            if(input_sum == DENDRITE_SIGNAL) {
                                changed = true;
                                grid[iz][iy][ix].type = DENDRITE;

                                for(int i=0; i<6; ++i) {
                                    if(grid[iz][iy][ix].iobuf[i] != 0) {
                                        grid[iz][iy][ix].gate = cell::adjacent_gate(i);
                                    }
                    
                                    grid[iz][iy][ix].iobuf[i] = grid[iz][iy][ix].chromo_dir_choice(i, DENDRITE_SIGNAL, 0);
                                }

                                break;
                            }

                            // default (more than one dendrite signal and no axon signal)
                            grid[iz][iy][ix].iobuf.fill(0);
                            break;

                          case NEURON:
                            grid[iz][iy][ix].iobuf.fill(DENDRITE_SIGNAL);
                            grid[iz][iy][ix].iobuf_from_gate() = AXON_SIGNAL;
                            grid[iz][iy][ix].iobuf_from_adjacent_gate() = AXON_SIGNAL; 
                            break;

                        case AXON:
                            for(int i=0; i<6; ++i) {
                                grid[iz][iy][ix].iobuf[i] = grid[iz][iy][ix].chromo_dir_choice(i, AXON_SIGNAL, 0);
                            }
                            break;

                        case DENDRITE:
                            for(int i=0; i<6; ++i) {
                                grid[iz][iy][ix].iobuf[i] = grid[iz][iy][ix].chromo_dir_choice(i, DENDRITE_SIGNAL, 0);
                            }
                            break;

                        default:
                            std::cerr << "something else wtf" << std::endl;
                            exit(1);
                            break;
                    }
                }
            }
        }

        kicking(); 
    }


    /* Signals are distributed from the neuron bodies via their axon tree 
     * and collected from connection dendrites.
     */
    void signal_step()
    {
        std::uint8_t input_sum { 0 };

        for(int iz=0; iz<GSize; ++iz) {
            for(int iy=0; iy<GSize; ++iy) {
                for(int ix=0; ix<GSize; ++ix) {
                    switch(grid[iz][iy][ix].type) {
                        case BLANK: break;

                        /* The neurons sum the incoming signal values and fire after a threshold is reached. 
                         * This behavior of the neuron bodies can be modified easily to suit a given problem.
                         * The output of the neuron bodies is passed on to its surrounding axon cells. 
                         * These two types of cell-to-cell interaction cover all kinds of cell encounters.
                         */
                        case NEURON:
                            input_sum = 1 // add default gain
                                 + fold_plus(grid[iz][iy][ix].iobuf)
                                 - grid[iz][iy][ix].iobuf_from_gate()
                                 - grid[iz][iy][ix].iobuf_from_adjacent_gate();
                            
                            grid[iz][iy][ix].iobuf.fill(0);
                            grid[iz][iy][ix].activation += input_sum;
                            
                             // Fire now.
                            if(grid[iz][iy][ix].activation > 31) {
                                grid[iz][iy][ix].iobuf_from_gate() = 1;
                                grid[iz][iy][ix].iobuf_from_adjacent_gate() = 1;
                                grid[iz][iy][ix].activation = 0;
                            }
                            break;

                        case AXON:
                            grid[iz][iy][ix].iobuf.fill(grid[iz][iy][ix].iobuf_from_gate());
                            grid[iz][iy][ix].activation = (grid[iz][iy][ix].iobuf_from_gate() != 0) ? 1 : 0;
                            break;


                        case DENDRITE:
                            input_sum = fold_plus(grid[iz][iy][ix].iobuf);
                            input_sum = (input_sum > 2) ? 2 : input_sum;
                            grid[iz][iy][ix].iobuf.fill(0);
                            grid[iz][iy][ix].iobuf_from_gate() = input_sum;
                            grid[iz][iy][ix].activation = (input_sum != 0) ? 1 : 0;
                            break;
                    }
                }
            }
        }
        
        kicking(); 
    }


public:
    std::array<std::array<std::array<cell, GSize>, GSize>, GSize> grid;
    
    network() : changed(true), has_setup_signaling(false)
    {
        std::uniform_int_distribution<std::uint32_t> gsize_rng(0, GSize);

        for(int iz=0; iz<GSize; ++iz) {
            for(int iy=0; iy<GSize; ++iy) {
                for(int ix=0; ix<GSize; ++ix) {
                    // restrict to grid
                    if(((iz + 1) % 2) * (iy % 2) == 1) {
                        grid[iz][iy][ix].chromo = (grid[iz][iy][ix].chromo & ~3) | 12;
                    }

                    if((iz % 2) * ((iy + 1) % 2) == 1) {
                        grid[iz][iy][ix].chromo = (grid[iz][iy][ix].chromo & ~12) | 3;
                    }

                    // Kill unwanted neuronseeds. Neuronsee only on crossings.
                    if((iz % 2) + (iy % 2) != 0) {
                        grid[iz][iy][ix].chromo &= ~192;
                    }

                    // Decrease prob of neuronseeds
                    if(grid[iz][iy][ix].is_neuronseed()) {
                        if(gsize_rng(rng_gen) < (GSize / 2)) {
                            grid[iz][iy][ix].chromo &= ~192;
                        }
                    }

                    // restrict axon-initial-growth in neuron to XY-plane.
                    if(grid[iz][iy][ix].is_neuronseed()) {
                        grid[iz][iy][ix].chromo = (grid[iz][iy][ix].chromo & 192) | ((grid[iz][iy][ix].chromo & 63) % 4);
                    }
                }
            }
        }
    }


    void step_ca()
    {
        if(changed) {
            growth_step();
        } else {
            if(!has_setup_signaling) {
                setup_signaling();
            }

            signal_step();
        }
    }
};

#endif
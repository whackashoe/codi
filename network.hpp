#ifndef NETWORK_H
#define NETWORK_H

#include <array>
#include <bitset>
#include <numeric>
#include <random>
#include <iostream>
#include "cell.hpp"
#include "cell_type.hpp"

extern std::mt19937 rng_gen;

template <unsigned int GSize>
class network
{
public:
	bool changed;
  	int input_sum;
  	bool signaling_inited;
  	int count_ca_steps;
  	int max_steps;
	std::array<std::array<std::array<cell, GSize>, GSize>, GSize> grid;

	network() : 
		changed(true), 
		input_sum(0), 
		signaling_inited(false), 
		count_ca_steps(0),
		max_steps(100)
	{
		std::uniform_int_distribution<int> gsize_rng(0, GSize);

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
	  				if(grid[iz][iy][ix].chromo >> 6 == NEURONSEED) {
	  					if(gsize_rng(rng_gen) < (GSize / 2)) {
	  						grid[iz][iy][ix].chromo &= ~192;
	  					}
	  				}

	  				// restrict axon-initial-growth in neuron to XY-plane.
	  				if(grid[iz][iy][ix].chromo >> 6 == NEURONSEED) {
	  					grid[iz][iy][ix].chromo = (grid[iz][iy][ix].chromo & 192) | ((grid[iz][iy][ix].chromo & 63) % 4);
	  				}
	  		  	}
	  		}
		}
	}

	/* For the Neighborhood interaction.
	 * Names for the buffer correspond to the I-Buf,
	 * so where the Information (signal) came from;
	 * I.g. in the north buffer (+y=2) of a cell is either what came
	 * from the norht (after kick) or the what will go to the south 
	 * (before kick).
	 * iobuf[0..5] = east(+x), west(-x), north(+y), south(-y),
	 * top(+z), bottom(-z) 
	 * actulize the 3 direction one at a time, without wrap-around.
	 */
	void kicking()
	{
	    // For the positive directions 
	    for(int iz=0; iz<GSize; iz++) {
	    	for(int iy=0; iy<GSize; iy++) {
				for(int ix=0; ix<GSize; ix++) {
			   		grid[iz][iy][ix].iobuf[0] = (ix != GSize-1) ? grid[iz+1][iy][ix].iobuf[0] : 0;
			    	grid[iz][iy][ix].iobuf[2] = (iy != GSize-1) ? grid[iz][iy+1][ix].iobuf[2] : 0;
			  		grid[iz][iy][ix].iobuf[4] = (iz != GSize-1) ? grid[iz][iy][ix+1].iobuf[4] : 0;

		    		// For the negtive directions 
		    		// CASpaceTemp pionts to the last cell now 
		    		for(int iz=GSize-1; iz >= 0; iz--) {
		      			for(int iy=GSize-1; iy >= 0; iy--) {
							for(int ix=GSize-1; ix >= 0; ix--) {
				  				grid[iz][iy][ix].iobuf[1] = (ix != 0) ? grid[iz-1][iy][ix].iobuf[1] : 0;
				    			grid[iz][iy][ix].iobuf[3] = (iy != 0) ? grid[iz][iy-1][ix].iobuf[3] : 0;
				    			grid[iz][iy][ix].iobuf[5] = (iz != 0) ? grid[iz][iy][ix-1].iobuf[5] : 0;
				    		}
				    	}
				    }
				}
			}
		}
	}

	void init_signaling()
	{
		std::uniform_int_distribution<int> three_two_rng(0, 32);
    	signaling_inited = true;

    	for(int iz=0; iz<GSize; iz++) {
	    	for(int iy=0; iy<GSize; iy++) {
				for(int ix=0; ix<GSize; ix++) {
			  		grid[iz][iy][ix].activation = 0;
			  		grid[iz][iy][ix].iobuf.fill(0);

			  		if(grid[iz][iy][ix].type == NEURON) {
			    		grid[iz][iy][ix].activation = three_two_rng(rng_gen);
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
      		if(!signaling_inited) {
      			init_signaling();
	      	}

    		signal_step();
    	}
  	}

  	/* now the rules 
	 * the chromo is a bitmask for the 6 directions[Bit0..5]:
	 * east(+x), west(-x), north(+y), south(-y),
	 * top(+z), bottom(-z) 
	 * As the direction are to be seen as input directios,
	 * the gate and chromo are masks that invert the directions
	 * when applied to output selection. 
	 */
	void growth_step()
  	{ 
    	changed = false;

    	for(int iz=0; iz<GSize; iz++) {
      		for(int iy=0; iy<GSize; iy++) {
				for(int ix=0; ix<GSize; ix++) {
	  				switch(grid[iz][iy][ix].type) {
	  					case BLANK: 
					    	// see if it is a neuronseed (in bit ..7,8 of chromo) 
					    	if((grid[iz][iy][ix].chromo >> 6) == NEURONSEED) {
					      		grid[iz][iy][ix].type = NEURON;
					      		changed = true;
					      		// and inform the neighbors immediately 
					      		grid[iz][iy][ix].gate = (grid[iz][iy][ix].chromo & 63) % 6;
								grid[iz][iy][ix].iobuf.fill(DENDRITE_SIG);
					     		grid[iz][iy][ix].iobuf[grid[iz][iy][ix].gate] = AXON_SIG;
					      		grid[iz][iy][ix].iobuf[((grid[iz][iy][ix].gate % 2) * -2) + 1 + grid[iz][iy][ix].gate] = AXON_SIG;
					    		break;
					    	}

						    // test for no signal 
						    input_sum = std::accumulate(
						    	std::begin(grid[iz][iy][ix].iobuf), std::end(grid[iz][iy][ix].iobuf), 
    							0, [](int a, int b) { return a + b; }
						    );
							if(input_sum == 0) {
								break;
							}

							// test for axon signals
							input_sum = std::accumulate(
						    	std::begin(grid[iz][iy][ix].iobuf), std::end(grid[iz][iy][ix].iobuf), 
    							0, [](int a, int b) { return a + (b & AXON_SIG); }
						    );
							// exactly one AXON_SIG
							if(input_sum == AXON_SIG) {
	      						grid[iz][iy][ix].type = AXON;
							    changed = true;

							    for(int i=0; i<6; i++) {
									if(grid[iz][iy][ix].iobuf[i] == AXON) {
								  		grid[iz][iy][ix].gate = i;
									}
									grid[iz][iy][ix].iobuf[i] = (((grid[iz][iy][ix].chromo >> i) & 1) != 0) ? AXON_SIG : 0;
								}
								break;
							}
							// more than one AXON_SIG 
							if(input_sum > AXON_SIG) {
								grid[iz][iy][ix].iobuf.fill(0);
							    break;
							}

						    // Test for DENDRITE_SIG's 
						    input_sum = std::accumulate(
						    	std::begin(grid[iz][iy][ix].iobuf), std::end(grid[iz][iy][ix].iobuf), 
    							0, [](int a, int b) { return a + (b & DENDRITE_SIG); }
						    );
							// exactly one DENDRITE_SIG
							if(input_sum == DENDRITE_SIG) {
								changed = true;
								grid[iz][iy][ix].type = DENDRITE;

								for(int i=0; i<6; i++) {
									if(grid[iz][iy][ix].iobuf[i] != 0) {
			  							grid[iz][iy][ix].gate = ((i % 2) * -2) + 1 + i;
									}
								}

								for(int i=0; i<6; i++) {
									grid[iz][iy][ix].iobuf[i] = (((grid[iz][iy][ix].chromo >> i) & 1) != 0) ? DENDRITE_SIG : 0;
								}

								break;
							}

	    					// default(more than one DENDRITE_SIG and no AXON_SIG)
	    					grid[iz][iy][ix].iobuf.fill(0);
	    					break;

	  					case NEURON:
							grid[iz][iy][ix].iobuf.fill(DENDRITE_SIG);
					    	grid[iz][iy][ix].iobuf[grid[iz][iy][ix].gate] = AXON_SIG;
					    	grid[iz][iy][ix].iobuf[((grid[iz][iy][ix].gate % 2) * -2) + 1 + grid[iz][iy][ix].gate] = AXON_SIG; 
					   		break;

						case AXON:
							for(int i=0; i<6; i++) {
								grid[iz][iy][ix].iobuf[i] = (((grid[iz][iy][ix].chromo >> i) & 1) != 0) ? AXON_SIG : 0;
							}
							break;

						case DENDRITE:
	    					for(int i=0; i<6; i++) {
	    						grid[iz][iy][ix].iobuf[i] = (((grid[iz][iy][ix].chromo >> i) & 1) != 0) ? DENDRITE_SIG : 0;
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

	void signal_step()
	{
	    // feed in some activity 
    	// appFeedIn();

    	for(int iz=0;iz<GSize;iz++) {
      		for(int iy=0;iy<GSize;iy++) {
				for(int ix=0;ix<GSize;ix++) {
					// // now the Rules ////
					switch(grid[iz][iy][ix].type) {
						case BLANK:
							//for(int i=0; i<6; i++) 
							//grid[iz][iy][ix].iobuf[i] = 0;
							break;

						case NEURON:
							// add default gain
							input_sum = std::accumulate(
						    	std::begin(grid[iz][iy][ix].iobuf), std::end(grid[iz][iy][ix].iobuf), 
    							1, [](int a, int b) { return a + b; }
						    ) - grid[iz][iy][ix].iobuf[grid[iz][iy][ix].gate]
						      - grid[iz][iy][ix].iobuf[((grid[iz][iy][ix].gate % 2) * -2) + 1 + grid[iz][iy][ix].gate];
							
							grid[iz][iy][ix].iobuf.fill(0);
							grid[iz][iy][ix].activation += input_sum;
							
							 // Fire now.
							if(grid[iz][iy][ix].activation > 31) {
							   grid[iz][iy][ix].iobuf[grid[iz][iy][ix].gate] = 1;
							   grid[iz][iy][ix].iobuf[((grid[iz][iy][ix].gate % 2) * -2) + 1 + grid[iz][iy][ix].gate] = 1;
							   grid[iz][iy][ix].activation = 0;
							}
						  	break;

						case AXON:
							for(int i=0; i<6; i++) {
						    	grid[iz][iy][ix].iobuf[i] = grid[iz][iy][ix].iobuf[grid[iz][iy][ix].gate];
							}
							grid[iz][iy][ix].activation = ((grid[iz][iy][ix].iobuf[grid[iz][iy][ix].gate]) != 0) ? 1 : 0;
						 	break;

						case DENDRITE:
							input_sum = std::accumulate(
						    	std::begin(grid[iz][iy][ix].iobuf), std::end(grid[iz][iy][ix].iobuf), 
    							0, [](int a, int b) { return a + b; }
						    );
							input_sum = (input_sum > 2) ? 2 : input_sum;
						    grid[iz][iy][ix].iobuf.fill(0);
						  	grid[iz][iy][ix].iobuf[grid[iz][iy][ix].gate] = input_sum;
						  	
						  	grid[iz][iy][ix].activation = (input_sum != 0) ? 1 : 0;
							break;
					}
				}
			}
		}
		
		kicking(); 
	}

	void render()
	{
		int ix = 0;
		for(int iz=0; iz<GSize; iz++) {
			for(int iy=0; iy<GSize; iy++) {
				std::cout << grid[iz][iy][ix].type;
			}
			std::cout << std::endl;
		}

		std::cout << std::endl << std::endl << "==================" << std::endl;
	}
};

#endif
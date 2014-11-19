#ifndef CELL_H
#define CELL_H

#include <array>
#include <bitset>
#include <iostream>
#include "cell_type.hpp"

extern std::mt19937 rng_gen;
std::uniform_int_distribution<int> two_five_six_rng(0, 256);

class cell
{
public:
	cell_type type;
	
	int activation; //activation counter
	
	/* the chromo is a bitmask for the 6 directions[Bit0..5]:
	 * east(+x), west(-x), north(+y), south(-y),
	 * top(+z), bottom(-z) 
	 */
	int chromo;
	
	/* Every cell has a gate, which is interpreted differently depending on the type of the cell. 
	 * A neuron cell uses this gate to store its orientation, 
	 * i.e. the direction in which the axon is pointing. 
	 * In an axon cell, the gate points to the neighbor from which the neural signals are received. 
	 * An axon cell accepts input only from this neighbor, but makes its own output available to all its neighbors. 
	 * In this way axon cells distribute information.
	 * the gate and Chromo are masks that invert the directions
	 * when applied to output selection. 
	 */
	int gate;

	/* iobuf stores the prior inputs for the neighborhood interaction
	*/
	std::array<int, 6> iobuf;  /*{ N, W, S, E, U, D ;}*/


	cell() : 
		type(BLANK), 
		activation(0), 
		chromo(two_five_six_rng(rng_gen)), 
		gate(0), 
		iobuf({{0, 0, 0, 0, 0, 0}}) 
		{ }
};

#endif
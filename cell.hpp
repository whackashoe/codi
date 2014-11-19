#ifndef CELL_H
#define CELL_H

#include <array>
#include <iostream>
#include <cstdint>
#include "cell_type.hpp"

extern std::mt19937 rng_gen;
std::uniform_int_distribution<std::uint8_t> two_five_six_rng(0, 255);

class cell
{
public:
	/* iobuf stores the prior inputs for the neighborhood interaction
	*/
	std::array<std::uint8_t, 6> iobuf;  /*{ N, W, S, E, U, D ;}*/

	cell_type type;

	std::uint8_t activation; //activation counter
	
	/* the chromo is a bitmask for the 6 directions[Bit0..5]:
	 * east(+x), west(-x), north(+y), south(-y),
	 * top(+z), bottom(-z) 
	 */
	std::uint8_t chromo;
	
	/* Every cell has a gate, which is interpreted differently depending on the type of the cell. 
	 * A neuron cell uses this gate to store its orientation, 
	 * i.e. the direction in which the axon is pointing. 
	 * In an axon cell, the gate points to the neighbor from which the neural signals are received. 
	 * An axon cell accepts input only from this neighbor, but makes its own output available to all its neighbors. 
	 * In this way axon cells distribute information.
	 * the gate and Chromo are masks that invert the directions
	 * when applied to output selection. 
	 */
	std::uint8_t gate;



	cell() : 
		iobuf({{0, 0, 0, 0, 0, 0}}),
		type(cell_type::BLANK), 
		activation(0), 
		chromo(two_five_six_rng(rng_gen)), 
		gate(0)
		{ }
};

#endif
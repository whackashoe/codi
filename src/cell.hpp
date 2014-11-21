#ifndef CELL_H
#define CELL_H

#include <array>
#include <iostream>
#include <cstdint>
#include "utility.hpp"
#include "cell_type.hpp"


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


    /* gets "left" from "right" and "right" from "left"
     * ie. if we are given left we return right
     *                 given up we return down
     *                 given south we return north
     */
    inline std::uint8_t adjacent_gate() const
    {
        return ((gate % 2) * -2) + 1 + gate;
    }

    inline static std::uint8_t adjacent_gate(const std::uint8_t gate);

    /* see if it is a neuronseed (in bit ..7,8 of chromo) 
     */
    inline bool is_neuronseed() const
    {
        return (chromo >> 6) == NEURONSEED;
    }

    /* return 'a' if chromo dir is right, 'b' if chromo diris left / odd->'a', even->'b'
     * dir is from shift ie X(east, west) Y(north, south) Z(top bottom)
     * read chromo bitmask note above
    */
    inline std::uint8_t chromo_dir_choice(const int shift, const std::uint8_t a, const std::uint8_t b) const
    {
        return (((chromo >> shift) & 1) != 0) ? a : b;
    }

    inline std::uint8_t & iobuf_from_gate()
    {
        return iobuf[gate];
    }

    inline std::uint8_t iobuf_from_gate() const
    {
        return iobuf[gate];
    }

    inline std::uint8_t & iobuf_from_adjacent_gate()
    {
        return iobuf[adjacent_gate()];
    }

    inline std::uint8_t iobuf_from_adjacent_gate() const
    {
        return iobuf[adjacent_gate()];
    }
};

std::uint8_t cell::adjacent_gate(const std::uint8_t gate)
{
    return ((gate % 2) * -2) + 1 + gate;
}

#endif
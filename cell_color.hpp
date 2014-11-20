#ifndef CELL_COLOR_H
#define CELL_COLOR_H

#include <cstdint>
#include <SFML/Graphics.hpp>


enum class cell_color : std::uint8_t {
	DEFAULT,
	AXON_SIGNAL,
	DENDRITE_SIGNAL,
	NEURON,
	AXON,
	DENDRITE
};

#endif
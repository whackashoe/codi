#include <iostream>

#include <random>
#include "network.hpp"

std::random_device rd;
std::mt19937 rng_gen(rd());

int main(int argc, char ** argv)
{
	std::random_device rd;
	std::mt19937 rng_gen(rd());

	network<16> nw;

	for(int i=0; i<10; i++) {
		nw.render();
		nw.step_ca();
	}

    return 0;
}

#include <iostream>
#include <random>
#include "network.hpp"

constexpr int GSize = 64;
constexpr int Max_Iterations = 100;

std::random_device rd;
std::mt19937 rng_gen(rd());

network<GSize> nw;

int main(int argc, char ** argv)
{
	for(int i=0; i<Max_Iterations; i++) {
		std::cout << i << std::endl;
		nw.step_ca();
		nw.render();
	}

    return 0;
}

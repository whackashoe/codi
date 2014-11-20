#include <iostream>
#include <random>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "config.hpp"
#include "network.hpp"
#include "cell_color.hpp"


std::random_device rd;
std::mt19937 rng_gen { rd() };
std::uniform_int_distribution<std::uint8_t> two_five_six_rng { 0, 255 };
network<GSize> nw;


void render_ascii(const network<GSize> & nw)
{
	int ix { 0 };

	for(int iz=0; iz<GSize; ++iz) {
		for(int iy=0; iy<GSize; ++iy) {
			char c;
			switch(nw.grid[iz][iy][ix].type) {
				case BLANK:    c = ' '; break;
				case NEURON:   c = '@'; break;
				case AXON:     c = '*'; break;
				case DENDRITE: c = '.'; break;
			}
			std::cout << c;
		}
		std::cout << std::endl;
	}

	std::cout << std::endl << std::endl;
}


sf::Color cell_to_color(cell_color c)
{
	const sf::Color signal_color(sf::Color::Yellow);
	const sf::Color neuron_color(sf::Color::White);
	const sf::Color axon_color(sf::Color::Red);
	const sf::Color dendrite_color(sf::Color::Green);
	const sf::Color default_color(sf::Color::Blue);
	
	switch(c) {
		case cell_color::SIGNAL:   return signal_color;   break;
		case cell_color::NEURON:   return neuron_color;   break;
		case cell_color::AXON: 	   return axon_color;     break;
		case cell_color::DENDRITE: return dendrite_color; break;
		default:                   return default_color;  break;
	}
}


bool render_2d(sf::RenderWindow & window, network<GSize> & nw)
{	 
	sf::Event event;
	
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			window.close();
			return false;
		}
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
		return false;
	}

	window.clear(sf::Color::Black);

	uint sg_w = std::floor(window_width / GSize);
	uint sg_h = std::floor(window_height / GSize);
	int ix { 0 };

	for(int iz=0; iz<GSize; ++iz) {
		for(int iy=0; iy<GSize; ++iy) {
			if(nw.grid[iz][iy][ix].type != BLANK) {
				sf::RectangleShape square(sf::Vector2f(sg_w, sg_h));
				cell_color color;

				if(nw.grid[iz][iy][ix].activation != 0) {
					if(nw.grid[iz][iy][ix].type != NEURON) {
						color = cell_color::SIGNAL;
					} else {
						color = cell_color::NEURON;
					}
				} else {
					switch(nw.grid[iz][iy][ix].type) {
						case NEURON:   color = cell_color::NEURON;   break;
						case AXON:     color = cell_color::AXON;     break;
						case DENDRITE: color = cell_color::DENDRITE; break;
					}
				}

				if(nw.render_space[iz][iy][iz] != color) {
					nw.render_space[iz][iy][iz] = color;
					square.setFillColor(cell_to_color(color));
					square.setPosition((sg_w*iz), (sg_h*iy));
					window.draw(square);
				}
			}
		}
	}

	window.display();
	return true;
} 



int main(int argc, char ** argv)
{
	sf::RenderWindow window{{window_width, window_height}, "rnn"};
	for(int i=0; i<Max_Iterations; i++) {
		std::cout << i << std::endl;
		nw.step_ca();
		//render_ascii(nw);
		if(!render_2d(window, nw)) {
			exit(0);
		}
	}

	return 0;
}

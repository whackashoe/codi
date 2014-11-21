#include <iostream>
#include <iomanip>
#include <random>
#include <array>
#include <sstream>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>


#include "config.hpp"
#include "network.hpp"
#include "cell_color.hpp"


network<GSize> nw;

void render_ascii(const network<GSize> & nw)
{
    int ix { 0 };

    for(int iz=0; iz<GSize; ++iz) {
        for(int iy=0; iy<GSize; ++iy) {
            char c { ' ' };
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


sf::Color cell_type_to_color(const cell_color c)
{
    const sf::Color neuron_color          (217, 184, 0,   255);
    const sf::Color axon_color            (150, 50,  150, 150);
    const sf::Color axon_signal_color     (255, 0,   200, 255); 
    const sf::Color dendrite_color        (50,  150, 150, 150);
    const sf::Color dendrite_signal_color (0,   255, 200, 255);
    const sf::Color default_color         (60,  60,  60,  255);
    
    switch(c) {
        case cell_color::DEFAULT:         return default_color;         break;
        case cell_color::AXON_SIGNAL:     return axon_signal_color;     break;
        case cell_color::DENDRITE_SIGNAL: return dendrite_signal_color; break;
        case cell_color::NEURON:          return neuron_color;          break;
        case cell_color::AXON:            return axon_color;            break;
        case cell_color::DENDRITE:        return dendrite_color;        break;
        default:                          return default_color;         break;
    }
}


bool render_2d(
    sf::RenderWindow & window,
    const network<GSize> & nw, 
    std::array<std::array<cell_color, GSize>, GSize> & render_space
) {
    static int ix { 0 };
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
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        --ix;
        if(ix == -1) ix = GSize-1;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        ++ix;
        if(ix == GSize-1) ix = 0;
    }

    uint sg_w = std::floor(window_width / GSize);
    uint sg_h = std::floor(window_height / GSize);

    for(int iz=0; iz<GSize; ++iz) {
        for(int iy=0; iy<GSize; ++iy) {
            if(nw.grid[iz][iy][ix].type != BLANK) {
                sf::RectangleShape square { sf::Vector2f(sg_w, sg_h) };
                cell_color color { cell_color::DEFAULT };

                if(nw.grid[iz][iy][ix].activation != 0) {
                    if(nw.grid[iz][iy][ix].type != NEURON) {
                        switch(nw.grid[iz][iy][ix].type) {
                            case AXON:     color = cell_color::AXON_SIGNAL;     break;
                            case DENDRITE: color = cell_color::DENDRITE_SIGNAL; break;
                        }
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

                square.setFillColor(cell_type_to_color(render_space[iz][iy]));
                square.setPosition((sg_w*iz), (sg_h*iy));
                window.draw(square);

                if(render_space[iz][iy] != color) {
                    render_space[iz][iy] = color;
                    square.setFillColor(cell_type_to_color(color));
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
    window.setFramerateLimit(10);
    window.clear(cell_type_to_color(cell_color::DEFAULT));
    
    std::array<std::array<cell_color, GSize>, GSize> render_space;

    for(int i=0; ; ++i) {
        std::cout << i << std::endl;
        nw.step_ca();
        //render_ascii(nw);
        if(!render_2d(window, nw, render_space)) {
            break;
        }
    }

    return 0;
}

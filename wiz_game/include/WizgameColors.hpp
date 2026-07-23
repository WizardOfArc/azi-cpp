#pragma once

#include "SFML/Graphics/Color.hpp"
#include <cstddef>

const sf::Color colorPallette[] = {
    sf::Color(255,255,255),
    sf::Color(255,225,255),
    sf::Color(255,195,255),
    sf::Color(255,165,255),
    sf::Color(255,135,255),
    sf::Color(255,105,255),
    sf::Color(255,75,255),
    sf::Color(255,45,255),
    sf::Color(255,15,255),
    sf::Color(255,0,255),
    sf::Color(255,0,225),
    sf::Color(255,0,195),
    sf::Color(255,0,165),
    sf::Color(255,0,135),
    sf::Color(255,0,105),
    sf::Color(255,0,75),
    sf::Color(255,0,45),
    sf::Color(255,0,15),
    sf::Color(255,0,0),
    sf::Color(255,0,0),
};

namespace wizgame {
    sf::Color chargeColor(float charge){
        auto capped = charge > 100.f ? 100.f : charge;
        auto normalized = static_cast<size_t>(capped / 5.f);
        return colorPallette[20 - normalized];
    }

    sf::Color ageColor(size_t phase){
        size_t normed;
        if(phase > 19){
            normed = 19;
        } else {
            normed = phase;
        }
        return colorPallette[normed];
    }
    
}
#pragma once

#include <SFML\Graphics\Font.hpp>

struct Font
{
    sf::Font stix_two_text;
    sf::Font smeshariki;
    sf::Font super_mario;
    sf::Font erica_type;
    sf::Font common;

    Font() {
        stix_two_text.loadFromFile("res/Fonts/stix_two_text.otf");
        smeshariki.loadFromFile("res/Fonts/smeshariki_regular.ttf");
        super_mario.loadFromFile("res/Fonts/super_mario.ttf");
        erica_type.loadFromFile("res/Fonts/erica_type.ttf");
        common.loadFromFile("res/Fonts/common.ttf");
    }

    ~Font() {

    }
};


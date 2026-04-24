#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <string>

class Enemy {
public:
    Enemy(const std::string& imagePath);
    void draw(sf::RenderWindow& window);
    void update();

private:
    sf::Texture texture;
    sf::Sprite  sprite;
    sf::Vector2f velocity;
};
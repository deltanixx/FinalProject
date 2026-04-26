#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include "World.hpp"

class Enemy {
public:
    Enemy(const std::string& imagePath);
    void draw(sf::RenderWindow& window);
    void update(float deltaTime);
    sf::FloatRect getBounds();

private:
    sf::Texture texture;
    sf::Sprite  sprite;
    sf::Vector2f velocity;

    const float gravity = 10.f;
};
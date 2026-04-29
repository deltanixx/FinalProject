#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include "World.hpp"

class Enemy {
public:
    Enemy(const std::string& imagePath);
    void draw(sf::RenderWindow& window);
    void update(float deltaTime, const World& world);
    sf::FloatRect getBounds();

private:
    sf::Texture  texture;
    sf::Sprite   sprite;
    sf::Vector2f position;
    sf::Vector2f size;
    sf::Vector2f velocity;
    bool         onGround = false;
    float        moveDir  = 1.f; // 1 = right, -1 = left

    static constexpr float gravity   = 500.f;
    static constexpr float moveSpeed = 60.f;
};

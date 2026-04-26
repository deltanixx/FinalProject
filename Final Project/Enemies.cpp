#include "Enemies.hpp"
#include <iostream>


Enemy::Enemy(const std::string& imagePath) : sprite(texture)
{
    if (!texture.loadFromFile(imagePath)) {
        std::cerr << "Texture failed" << std::endl;
    }
    else {
        std::cout << "Texture loaded" << std::endl;
    }
    sprite.setTexture(texture, true);
    sprite.setScale(sf::Vector2f(0.075f, 0.075f));

    float vx = (rand() % 1) - 0.05;
    float vy = 0; //(rand() % 7) - 3 - for y-movement
    velocity = sf::Vector2f(vx, vy);
}

void Enemy::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

sf::FloatRect Enemy::getBounds()
{
    return sprite.getGlobalBounds();
}

void Enemy::update(float deltaTime) //All movement related stuff for enemy
{
    sf::Vector2f pos = sprite.getPosition();

    //Physics
    velocity.y += gravity * deltaTime;

    if (pos.x < 0 || pos.x > 400) //Boundaries for x-position
        velocity.x = -velocity.x;
    if (pos.y < 0 || pos.y > 750) //Boundaries for y-position
        velocity.y = -velocity.y;

    sprite.move(velocity);
}
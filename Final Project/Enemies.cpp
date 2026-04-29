#include "Enemies.hpp"
#include <iostream>
#include <cstdlib>

Enemy::Enemy(const std::string& imagePath) : sprite(texture)
{
    if (!texture.loadFromFile(imagePath))
        std::cerr << "Texture failed" << std::endl;
    else
        std::cout << "Texture loaded" << std::endl;

    sprite.setTexture(texture, true);
    sprite.setScale(sf::Vector2f(0.075f, 0.075f));

    position = sf::Vector2f(300.f, 50.f);
    sprite.setPosition(position);
    size = sprite.getGlobalBounds().size;

    moveDir  = (rand() % 2 == 0) ? 1.f : -1.f;
    velocity = sf::Vector2f(0.f, 0.f);
}

void Enemy::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

/// Walks the enemy at constant speed, reversing direction on wall impact, and falls off ledges.
void Enemy::update(float deltaTime, const World& world)
{
    velocity.x = moveSpeed * moveDir;
    applyPhysics(deltaTime, world);

    if (velocity.x == 0.f)
        moveDir = -moveDir;

    sprite.setPosition(position);
}

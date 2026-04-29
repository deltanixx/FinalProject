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
    size     = sprite.getGlobalBounds().size;

    moveDir  = (rand() % 2 == 0) ? 1.f : -1.f;
    velocity = sf::Vector2f(0.f, 0.f);
}

void Enemy::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

sf::FloatRect Enemy::getBounds()
{
    return sprite.getGlobalBounds();
}

/// Applies gravity, resolves tile collisions, and walks the enemy v reversing direction on wall impact.
void Enemy::update(float deltaTime, const World& world)
{
    if (!onGround)
        velocity.y += gravity * deltaTime;

    velocity.x = moveSpeed * moveDir;

    world.resolveCollision(position, size, velocity, onGround, deltaTime);

    // Reverse direction if a horizontal tile collision stopped us
    if (velocity.x == 0.f)
        moveDir = -moveDir;

    sprite.setPosition(position);
}

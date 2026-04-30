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

void Enemy::update(float deltaTime, const World& world)
{
    velocity.x = moveSpeed * moveDir;
    applyPhysics(deltaTime, world);

    if (velocity.x == 0.f)
        moveDir = -moveDir;

    sprite.setPosition(position);
}

void Enemy::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

sf::Vector2f Enemy::getPosition() const
{
    return position;
}  

sf::Vector2f Enemy::setPosition(const sf::Vector2f& newPosition)
{
    position = newPosition;
    sprite.setPosition(position);

    return position;
}

bool Enemy::chasePlayer(sf::Vector2f playerPosition, sf::Vector2f enemyPosition)
{
    // Calculate distance to player
    float dx = playerPosition.x - enemyPosition.x;
    float dy = playerPosition.y - enemyPosition.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    // Chase range 
    float chaseRange = 3000.f;

    return (distance < chaseRange && distance > 0);
}

void Enemy::update(bool seePlayer, sf::Vector2f playerPosition, sf::Vector2f enemyPosition,
    float deltaTime, const World& world)
{
    applyPhysics(deltaTime, world);

    static float debugTimer = 0;
    debugTimer += deltaTime;
    if (debugTimer >= 1.0f) {  // Print every second
        float dx = playerPosition.x - enemyPosition.x;
        float dy = playerPosition.y - enemyPosition.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        std::cout << "=== Debug Info ===" << std::endl;
        std::cout << "seePlayer: " << seePlayer << std::endl;
        std::cout << "Distance to player: " << distance << std::endl;
        std::cout << "Chase range: 300" << std::endl;
        std::cout << "Is in range: " << (distance < 300) << std::endl;
        std::cout << "Enemy position: (" << enemyPosition.x << ", " << enemyPosition.y << ")" << std::endl;
        std::cout << "Player position: (" << playerPosition.x << ", " << playerPosition.y << ")" << std::endl;
        std::cout << "=================" << std::endl;

        debugTimer = 0;
    }


    if (!seePlayer)
    {
        // Patrol behavior
        velocity.x = moveSpeed * moveDir;

        if (velocity.x == 0.f)
            moveDir = -moveDir;

        sprite.setPosition(position);
    }
    else  // seePlayer == true
    {
        if (chasePlayer(playerPosition, enemyPosition))
        {
            // Calculate direction to player
            float dx = playerPosition.x - enemyPosition.x;
            float dy = playerPosition.y - enemyPosition.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            // Calculate direction to player
            sf::Vector2f direction(dx / distance, dy / distance);

            // Calculate movement for frame
            sf::Vector2f movement = direction * moveSpeed * deltaTime;

            // Store new position
            sf::Vector2f newPosition = position + movement;  // Use position, not enemyPosition
            setPosition(newPosition);
        }
        else
        {
            // Player seen but out of range - walk toward player slowly
            float dx = playerPosition.x - position.x;
            float dy = 0;  // Only horizontal movement for now
            if (dx > 0)
                moveDir = 1.f;
            else if (dx < 0)
                moveDir = -1.f;

            velocity.x = moveSpeed * moveDir * 0.5f;  // Half speed when approaching
            sprite.setPosition(position);
        }
    }
}
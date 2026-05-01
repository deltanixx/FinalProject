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

bool Enemy::isAlive()        const { return alive; }
bool Enemy::isBoss()         const { return isBoss_; }
int  Enemy::getContactDamage() const { return isBoss_ ? 25 : 10; }

void Enemy::kill() { alive = false; }

void Enemy::takeDamage(int amount)
{
    health -= amount;
    if (health <= 0) {
        health = 0;
        kill();
    }
}

void Enemy::setAsBoss()
{
    isBoss_           = true;
    health            = 500;
    maxHealth         = 500;
    instanceMoveSpeed = 50.f;
    sprite.setScale({ 0.225f, 0.225f });
    size = sprite.getGlobalBounds().size;
}

void Enemy::respawn(sf::Vector2f newPosition)
{
    alive    = true;
    health   = maxHealth;
    velocity = sf::Vector2f(0.f, 0.f);
    setPosition(newPosition);
}

void Enemy::drawHealthBar(sf::RenderWindow& window)
{
    if (!alive) return;

    const float barH = isBoss_ ? 10.f : 6.f;
    const float barW = size.x;
    const float barX = position.x;
    const float barY = position.y - barH - 4.f;

    sf::RectangleShape bg({ barW, barH });
    bg.setPosition({ barX, barY });
    bg.setFillColor(sf::Color(60, 60, 60));
    window.draw(bg);

    float fillRatio = static_cast<float>(health) / static_cast<float>(maxHealth);
    sf::RectangleShape fill({ barW * fillRatio, barH });
    fill.setPosition({ barX, barY });
    fill.setFillColor(isBoss_ ? sf::Color(200, 0, 220) : sf::Color(50, 200, 50));
    window.draw(fill);

    sf::RectangleShape border({ barW, barH });
    border.setPosition({ barX, barY });
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color::White);
    border.setOutlineThickness(1.f);
    window.draw(border);
}

void Enemy::update(float deltaTime, const World& world)
{
    if (!alive) return;

    velocity.x = instanceMoveSpeed * moveDir;
    applyPhysics(deltaTime, world);

    if (velocity.x == 0.f)
        moveDir = -moveDir;

    sprite.setPosition(position);
}

void Enemy::draw(sf::RenderWindow& window)
{
    if (!alive) return;
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
    float dx = playerPosition.x - enemyPosition.x;
    float dy = playerPosition.y - enemyPosition.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    float chaseRange = 30.f;
    return (distance < chaseRange && distance > 0);
}

void Enemy::update(bool seePlayer, sf::Vector2f playerPosition, sf::Vector2f enemyPosition,
    float deltaTime, const World& world)
{
    applyPhysics(deltaTime, world);

    if (!seePlayer)
    {
        velocity.x = moveSpeed * moveDir;
        if (velocity.x == 0.f)
            moveDir = -moveDir;
        sprite.setPosition(position);
    }
    else
    {
        if (chasePlayer(playerPosition, enemyPosition))
        {
            float dx = playerPosition.x - enemyPosition.x;
            float dy = playerPosition.y - enemyPosition.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            sf::Vector2f direction(dx / distance, dy / distance);
            sf::Vector2f newPosition = position + direction * moveSpeed * deltaTime;
            setPosition(newPosition);
        }
        else
        {
            float dx = playerPosition.x - position.x;
            if (dx > 0)
                moveDir = 1.f;
            else if (dx < 0)
                moveDir = -1.f;
            velocity.x = moveSpeed * moveDir;
            sprite.setPosition(position);
        }
    }
}
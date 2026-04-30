#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "PhysicsEntity.hpp"
#include "Player.hpp"
#include "World.hpp"
#include <cmath>

class Enemy : public PhysicsEntity{
public:
    Enemy(const std::string& imagePath);
    void draw(sf::RenderWindow& window) override;
    void update(float deltaTime, const World& world) override;
    void update(bool seePlayer, sf::Vector2f playerPosition, sf::Vector2f enemyPosition, float deltaTime, const World& world);
    sf::Vector2f getPosition() const;
    sf::Vector2f setPosition(const sf::Vector2f& newPosition);

    bool chasePlayer(sf::Vector2f playerPosition, sf::Vector2f enemyPosition);
private:
    sf::Texture texture;
    sf::Sprite  sprite;
    float       moveDir = 1.f;

    sf::Vector2f enemyPosition; //For enemy pathfinding

    static constexpr float moveSpeed = 25.f;
};

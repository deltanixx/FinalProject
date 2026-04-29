#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "PhysicsEntity.hpp"

class Enemy : public PhysicsEntity {
public:
    Enemy(const std::string& imagePath);
    void draw(sf::RenderWindow& window) override;
    void update(float deltaTime, const World& world) override;

private:
    sf::Texture texture;
    sf::Sprite  sprite;
    float       moveDir = 1.f;

    static constexpr float moveSpeed = 25.f;
};

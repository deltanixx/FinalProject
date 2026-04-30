#pragma once
#include <SFML/Graphics.hpp>
#include "PhysicsEntity.hpp"

class Player : public PhysicsEntity {
public:
    Player();
    void draw(sf::RenderWindow& window) override;
    void update(float deltaTime, const World& world) override;

    sf::Vector2f getPosition() const;

private:
    sf::Texture idleTexture;
    sf::Texture walkTexture;
    sf::Sprite  sprite;

    sf::Vector2f playerPosition; //For enemy pathfinding

    int health;

    sf::Vector2f spriteOffset; // offsets sprite render from physics box to compensate for texture padding

    float playerScale = 8.f; 
    float animTimer   = 0.f;
    bool  showWalk    = false;
    bool  facingRight = true;

    static constexpr float moveSpeed    = 200.f;
    static constexpr float jumpStrength = -200.f;
    static constexpr float animInterval = 0.15f;

    void updateAnimation(bool isMoving, float deltaTime);
};

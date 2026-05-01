#pragma once
#include <SFML/Graphics.hpp>
#include "PhysicsEntity.hpp"

class Player : public PhysicsEntity {
public:
    Player();
    void draw(sf::RenderWindow& window) override;
    void update(float deltaTime, const World& world) override;
    void drawHUD(sf::RenderWindow& window);

    sf::Vector2f getPosition() const;
    void setSpawnPosition(sf::Vector2f pos);
    void takeDamage(int amount);
    bool isDead() const;
    void respawn();
    int getHealth() const;
    int getMaxHealth() const;

private:
    sf::Texture idleTexture;
    sf::Texture walkTexture;
    sf::Sprite  sprite;

    sf::Vector2f playerPosition;
    sf::Vector2f spawnPosition;

    int health    = 100;
    int maxHealth = 100;
    float damageCooldown = 0.f;
    static constexpr float damageCooldownMax = 1.f;

    sf::Vector2f spriteOffset;

    float playerScale = 8.f;
    float animTimer   = 0.f;
    bool  showWalk    = false;
    bool  facingRight = true;

    static constexpr float moveSpeed    = 200.f;
    static constexpr float jumpStrength = -200.f;
    static constexpr float animInterval = 0.15f;

    void updateAnimation(bool isMoving, float deltaTime);
};

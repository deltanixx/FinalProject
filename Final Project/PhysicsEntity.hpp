#pragma once
#include <SFML/Graphics.hpp>
#include "World.hpp"

/// Base class providing shared gravity and tile-collision physics for all game entities.
class PhysicsEntity {
public:
    virtual ~PhysicsEntity() = default;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void update(float deltaTime, const World& world) = 0;
    sf::FloatRect getBounds() const;
    void setPosition(sf::Vector2f pos);

protected:
    sf::Vector2f position;
    sf::Vector2f size;
    sf::Vector2f velocity;
    bool onGround = false;

    static constexpr float gravity = 800.f;

    /// Applies gravity then resolves tile collisions, updating position and velocity in place.
    void applyPhysics(float deltaTime, const World& world);
};

#include "PhysicsEntity.hpp"

sf::FloatRect PhysicsEntity::getBounds() const
{
    return sf::FloatRect(position, size);
}

void PhysicsEntity::setPosition(sf::Vector2f pos)
{
    position = pos;
}

void PhysicsEntity::applyPhysics(float deltaTime, const World& world)
{
    if (!onGround)
        velocity.y += gravity * deltaTime;
    world.resolveCollision(position, size, velocity, onGround, deltaTime);
}

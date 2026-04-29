#include "Player.hpp"
#include <iostream>
#include <cmath>

Player::Player() : sprite(idleTexture)
{
    if (!idleTexture.loadFromFile("./Assets/Player/player_idle.png"))
        std::cerr << "Failed to load player idle" << std::endl;
    if (!walkTexture.loadFromFile("./Assets/Player/player_walking.png"))
        std::cerr << "Failed to load player walk" << std::endl;

    sprite.setTexture(idleTexture, true);

    sf::Vector2u texSize = idleTexture.getSize();
    float scale = (4.f * TILE_SIZE) / (float)texSize.y;
    sprite.setScale({ scale, scale });

    position = sf::Vector2f(200.f, 50.f);
    sprite.setPosition(position);
    size = sf::Vector2f((float)texSize.x * scale, (float)texSize.y * scale);
}

void Player::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

/// Swaps between idle and walk textures while moving, resets to idle when still.
void Player::updateAnimation(bool isMoving, float deltaTime)
{
    if (isMoving)
    {
        animTimer += deltaTime;
        if (animTimer >= animInterval)
        {
            showWalk  = !showWalk;
            animTimer = 0.f;
            sprite.setTexture(showWalk ? walkTexture : idleTexture, true);
        }
    }
    else
    {
        showWalk  = false;
        animTimer = 0.f;
        sprite.setTexture(idleTexture, true);
    }
}

/// Reads keyboard input, animates the sprite, flips it to face the direction of movement, and applies physics.
void Player::update(float deltaTime, const World& world)
{
    bool left  = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)  ||
                 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
    bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
                 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
    bool jump  = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) ||
                 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)    ||
                 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);

    velocity.x = 0.f;
    if (left)  { velocity.x -= moveSpeed; facingRight = false; }
    if (right) { velocity.x += moveSpeed; facingRight = true;  }

    if (jump && onGround)
        velocity.y = jumpStrength;

    updateAnimation(left || right, deltaTime);

    // Flip sprite to face direction — offset position compensates for negative x-scale pivot
    float absScaleX = std::abs(sprite.getScale().x);
    float scaleY    = sprite.getScale().y;
    if (facingRight)
    {
        sprite.setScale({ absScaleX, scaleY });
        sprite.setPosition(position);
    }
    else
    {
        sprite.setScale({ -absScaleX, scaleY });
        sprite.setPosition({ position.x + size.x, position.y });
    }

    applyPhysics(deltaTime, world);
    sprite.setPosition(facingRight ? position : sf::Vector2f(position.x + size.x, position.y));
}

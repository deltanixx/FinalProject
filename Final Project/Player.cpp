#include "Player.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

Player::Player() : sprite(idleTexture), swordSprite(swordTexture)
{
    if (!idleTexture.loadFromFile("./Assets/Player/player_idle.png"))
        std::cerr << "Failed to load player idle" << std::endl;
    if (!walkTexture.loadFromFile("./Assets/Player/player_walking.png"))
        std::cerr << "Failed to load player walk" << std::endl;

    sprite.setTexture(idleTexture, true);

    sf::Vector2u texSize = idleTexture.getSize();
    float scale = (this->playerScale * TILE_SIZE) / (float)texSize.y;
    sprite.setScale({ scale, scale });

    float fullW   = (float)texSize.x * scale;
    float fullH   = (float)texSize.y * scale;
    float topPad  = fullH * 0.07f;
    float botPad  = fullH * 0.17f;
    float sidePad = fullW * 0.09f;

    size         = sf::Vector2f(fullW - 2.f * sidePad, fullH - topPad - botPad);
    spriteOffset = sf::Vector2f(-sidePad, -topPad);

    position      = sf::Vector2f(200.f, 50.f);
    spawnPosition = position;
    sprite.setPosition(position + spriteOffset);

    // Sword sprite
    if (!swordTexture.loadFromFile("./Assets/Items/Greatsword of Death.png"))
        std::cerr << "Failed to load sword" << std::endl;
    swordSprite.setTexture(swordTexture, true);
    {
        sf::Vector2u sz         = swordTexture.getSize();
        float        swordScale = (playerScale * TILE_SIZE * 1.7f) / static_cast<float>(sz.y);
        swordSprite.setScale({ swordScale, swordScale });
        // Origin at the handle tip (bottom-left area of the diagonal sprite)
        swordSprite.setOrigin({ sz.x * 0.10f, sz.y * 0.88f });
    }

    // Generate a synthetic swoosh: frequency sweep 600 → 150 Hz with sine envelope
    {
        const unsigned   sampleRate = 44100;
        const float      dur        = 0.18f;
        const auto       n          = static_cast<std::uint64_t>(sampleRate * dur);
        std::vector<std::int16_t> samples(n);
        for (std::uint64_t i = 0; i < n; ++i) {
            float t        = static_cast<float>(i) / sampleRate;
            float progress = static_cast<float>(i) / static_cast<float>(n);
            float freq     = 600.f - 450.f * progress;
            float envelope = std::sin(progress * 3.14159265f) * 0.7f;
            samples[i]     = static_cast<std::int16_t>(
                                std::sin(2.f * 3.14159265f * freq * t) * envelope * 32767.f);
        }
        if (!swooshBuffer.loadFromSamples(samples.data(), n, 1, sampleRate, { sf::SoundChannel::Mono }))
            std::cerr << "Failed to generate swoosh sound" << std::endl;
        swooshSound.emplace(swooshBuffer);
    }
}

void Player::draw(sf::RenderWindow& window)
{
    for (const auto& p : projectiles)
        if (p.lifetime > 0.f)
            window.draw(p.sprite);
    window.draw(sprite);
    if (swordSwinging)
        window.draw(swordSprite);
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

void Player::setSpawnPosition(sf::Vector2f pos) { spawnPosition = pos; }
int  Player::getHealth()    const { return health; }
int  Player::getMaxHealth() const { return maxHealth; }
bool Player::isDead()       const { return health <= 0; }
bool Player::isSwordActive() const { return swordSwinging; }

sf::FloatRect Player::getSwordBounds() const
{
    if (!swordSwinging) return sf::FloatRect({}, {});
    const float reach = size.y * 2.5f;
    float x = facingRight
        ? position.x + size.x * 0.5f
        : position.x - reach + size.x * 0.5f;
    return sf::FloatRect({ x, position.y - size.y * 0.5f }, { reach, size.y * 2.0f });
}

void Player::updateProjectiles(float dt)
{
    for (auto& p : projectiles) {
        p.sprite.move(p.velocity * dt);
        p.lifetime -= dt;
    }
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const Projectile& p) { return p.lifetime <= 0.f; }),
        projectiles.end());
}

bool Player::consumeProjectileHit(const sf::FloatRect& target)
{
    for (auto& p : projectiles) {
        if (p.lifetime > 0.f && p.sprite.getGlobalBounds().findIntersection(target)) {
            p.lifetime = 0.f;
            return true;
        }
    }
    return false;
}

void Player::takeDamage(int amount)
{
    if (damageCooldown > 0.f) return;
    health -= amount;
    if (health < 0) health = 0;
    damageCooldown = damageCooldownMax;
}

void Player::respawn()
{
    health         = maxHealth;
    position       = spawnPosition;
    velocity       = sf::Vector2f(0.f, 0.f);
    damageCooldown = 0.f;
}

void Player::drawHUD(sf::RenderWindow& window)
{
    const float barW   = 200.f;
    const float barH   = 20.f;
    const float barX   = 20.f;
    const float barY   = 20.f;

    sf::RectangleShape bg({ barW, barH });
    bg.setPosition({ barX, barY });
    bg.setFillColor(sf::Color(50, 50, 50));
    window.draw(bg);

    float fillW = barW * (static_cast<float>(health) / maxHealth);
    sf::RectangleShape fill({ fillW, barH });
    fill.setPosition({ barX, barY });
    fill.setFillColor(sf::Color(220, 50, 50));
    window.draw(fill);

    sf::RectangleShape border({ barW, barH });
    border.setPosition({ barX, barY });
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color::White);
    border.setOutlineThickness(2.f);
    window.draw(border);
}

/// Reads keyboard input, animates the sprite, flips it to face the direction of movement, and applies physics.
void Player::update(float deltaTime, const World& world)
{
    if (damageCooldown > 0.f)
        damageCooldown -= deltaTime;

    updateProjectiles(deltaTime);

    if (isDead())
    {
        respawn();
        return;
    }

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

    // Sword swing — triggered by Z or left mouse; one swing per press
    bool attackPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) ||
                         sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    if (attackPressed && !swordSwinging && !swordJustHeld) {
        swordSwinging = true;
        swordTimer    = swordDuration;
        swordJustHeld = true;
        if (swooshSound) swooshSound->play();

        // Spawn a projectile shaped like the sword flying in the facing direction
        Projectile proj(swordTexture);
        {
            sf::Vector2u sz       = swordTexture.getSize();
            float        projScale = (playerScale * TILE_SIZE * 0.8f) / static_cast<float>(sz.y);
            proj.sprite.setScale({ projScale, projScale });
            proj.sprite.setOrigin({ sz.x * 0.5f, sz.y * 0.5f });
            sf::Vector2f hand = position + (facingRight
                ? sf::Vector2f(size.x * 0.75f, size.y * 0.35f)
                : sf::Vector2f(size.x * 0.25f, size.y * 0.35f));
            proj.sprite.setPosition(hand);
            const float projSpeed = 750.f;
            proj.velocity = { facingRight ? projSpeed : -projSpeed, -80.f };
            // +45° CW rotates the upper-right blade to point right; -135° points it left
            proj.sprite.setRotation(sf::degrees(facingRight ? 45.f : -135.f));
        }
        projectiles.push_back(std::move(proj));
    }
    if (!attackPressed)
        swordJustHeld = false;

    if (swordSwinging) {
        swordTimer -= deltaTime;
        if (swordTimer <= 0.f) {
            swordSwinging = false;
            swordTimer    = 0.f;
        }

        // progress 0 = start of swing, 1 = end
        float progress = 1.f - (swordTimer / swordDuration);
        float angle    = swordStartAngle + (swordEndAngle - swordStartAngle) * progress;

        // Pivot: player's hand — upper area, on the facing side
        sf::Vector2f pivot = position + (facingRight
            ? sf::Vector2f(size.x * 0.75f, size.y * 0.35f)
            : sf::Vector2f(size.x * 0.25f, size.y * 0.35f));

        float sc = std::abs(swordSprite.getScale().y);
        swordSprite.setScale({ facingRight ? sc : -sc, sc });
        swordSprite.setRotation(sf::degrees(facingRight ? angle : -angle));
        swordSprite.setPosition(pivot);
    }

    updateAnimation(left || right, deltaTime);

    applyPhysics(deltaTime, world);

    // Negative x-scale flips the sprite; when flipped, anchor shifts right by (size.x - spriteOffset.x)
    // so the visible character remains aligned with the physics box.
    float absScaleX = std::abs(sprite.getScale().x);
    float scaleY    = sprite.getScale().y;
    if (facingRight)
    {
        sprite.setScale({ -absScaleX, scaleY });
        sprite.setPosition({ position.x + size.x - spriteOffset.x, position.y + spriteOffset.y });
    }
    else
    {
        sprite.setScale({ absScaleX, scaleY });
        sprite.setPosition(position + spriteOffset);
    }
}

sf::Vector2f Player::getPosition() const 
{
    return position;
}
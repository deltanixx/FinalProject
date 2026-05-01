#pragma once
#include <array>
#include <optional>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "PhysicsEntity.hpp"

class Player : public PhysicsEntity {
public:
    Player();
    void draw(sf::RenderWindow& window) override;
    void update(float deltaTime, const World& world) override;
    void drawHUD(sf::RenderWindow& window);

    sf::Vector2f  getPosition()    const;
    void          setSpawnPosition(sf::Vector2f pos);
    void          takeDamage(int amount);
    bool          isDead()         const;
    void          respawn();
    int           getHealth()      const;
    int           getMaxHealth()   const;
    sf::FloatRect getSwordBounds()                             const;
    bool          isSwordActive()                              const;
    bool          consumeProjectileHit(const sf::FloatRect& target);
    void          setAimPosition(sf::Vector2f worldPos);

    void addToInventory(int tileType, int count);
    bool removeFromInventory(int tileType, int count);
    int  getInventoryCount(int tileType) const;
    int  getSelectedSlot() const;
    void setSelectedSlot(int slot);
    void drawHotbar(sf::RenderWindow& window, const sf::Font& font);

    static constexpr int hotbarSlotCount = 2;

private:

    sf::SoundBuffer hurtBuffer;
    sf::Sound hurtSound;

    sf::Texture idleTexture;
    sf::Texture walkTexture;
    sf::Sprite  sprite;

    // Sword — declared before swordSprite so origin is stable
    sf::Texture     swordTexture;
    sf::Sprite      swordSprite;
    sf::SoundBuffer          swooshBuffer;
    std::optional<sf::Sound> swooshSound;

    sf::Vector2f playerPosition;
    sf::Vector2f spawnPosition;
    sf::Vector2f aimPosition;

    int   health    = 100;
    int   maxHealth = 100;
    float damageCooldown = 0.f;
    static constexpr float damageCooldownMax = 1.f;

    bool  swordSwinging = false;
    bool  swordJustHeld = false;
    float swordTimer    = 0.f;
    static constexpr float swordDuration   = 0.35f;
    static constexpr float swordStartAngle = -90.f;
    static constexpr float swordEndAngle   =  80.f;

    struct Projectile {
        explicit Projectile(const sf::Texture& tex) : sprite(tex) {}
        sf::Sprite   sprite;
        sf::Vector2f velocity;
        float        lifetime = 2.f;
    };
    std::vector<Projectile> projectiles;
    void updateProjectiles(float dt);


    // Pickaxe — texture declared before sprite so init-list order is safe
    sf::Texture  pickaxeTexture_;
    sf::Sprite   pickaxeSprite_;
    bool  pickaxeSwinging_ = false;
    float pickaxeTimer_    = 0.f;
    static constexpr float pickaxeDuration_   = 0.28f;
    static constexpr float pickaxeStartAngle_ = -75.f;
    static constexpr float pickaxeEndAngle_   =  45.f;

    std::unordered_map<int, int> inventory_;
    int selectedSlot_ = 0;

    sf::Vector2f spriteOffset;

    float playerScale = 8.f;
    float animTimer   = 0.f;
    bool  showWalk    = false;
    bool  facingRight = true;

    static constexpr float moveSpeed    = 200.f;
    static constexpr float jumpStrength = -300.f;
    static constexpr float animInterval = 0.15f;

    void updateAnimation(bool isMoving, float deltaTime);
};

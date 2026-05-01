#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <string>
#include <vector>
#include "Enemies.hpp"
#include "Player.hpp"
#include "World.hpp"
#include "AssetManager.hpp"
#include "DayNightCycle.hpp"
#include "Music.hpp"
#include "Block.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 1280, 800 }), "Terraria 2");
    window.setFramerateLimit(60);

    Assets::loadTextures();
    Item::loadItems();

    // Day/night cycle
    DayNightCycle cycle(120.f);
    cycle.setSunTexture(&Assets::getTexture(3));
    cycle.setMoonTexture(&Assets::getTexture(4));

    World world = World();
    world.generateWorld(window);

    // Spawn enemies at random X positions
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    const int   NUM_ENEMIES = 4;
    const float worldW      = world.getSize().x;
    std::vector<std::unique_ptr<Enemy>> enemies;
    enemies.reserve(NUM_ENEMIES + 1); // +1 for eventual boss
    for (int i = 0; i < NUM_ENEMIES; ++i) {
        auto e = std::make_unique<Enemy>("./Assets/Enemy/Enemy.png");
        float rx = 100.f + static_cast<float>(std::rand() % static_cast<int>(worldW - 200.f));
        e->setPosition({ rx, 50.f });
        enemies.push_back(std::move(e));
    }

    float bossTimer    = 60.f;
    bool  bossSpawned  = false;

    Player player;
    float worldCenterX = worldW / 2.f;
    player.setPosition({ worldCenterX, 50.f });
    player.setSpawnPosition({ worldCenterX, 50.f });

    MusicPlayer music("./Assets/Music/theme.oga");
    music.play();

    sf::Font font;
    if (!font.openFromFile("./Assets/Fonts/PixelText.ttf"))
        std::cerr << "Failed to load font" << std::endl;
    sf::Text fpsText(font, "", 18);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setOutlineColor(sf::Color::Black);
    fpsText.setOutlineThickness(1.f);

    const sf::Vector2f viewSize(1280.f, 800.f);
    sf::View camera(sf::FloatRect({ 0.f, 0.f }, viewSize));

    sf::Clock deltaClock;

    while (window.isOpen())
    {
        float deltaTime = deltaClock.restart().asSeconds();

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        for (auto& e : enemies)
            e->update(deltaTime, world);
        player.update(deltaTime, world);

        // Boss spawn timer
        if (!bossSpawned) {
            bossTimer -= deltaTime;
            if (bossTimer <= 0.f) {
                auto boss = std::make_unique<Enemy>("./Assets/Enemy/Enemy.png");
                boss->setAsBoss();
                float rx = 100.f + static_cast<float>(std::rand() % static_cast<int>(worldW - 200.f));
                boss->setPosition({ rx, 50.f });
                enemies.push_back(std::move(boss));
                bossSpawned = true;
            }
        }

        // Sword melee hits enemy
        if (player.isSwordActive()) {
            sf::FloatRect sword = player.getSwordBounds();
            for (auto& e : enemies)
                if (e->isAlive() && sword.findIntersection(e->getBounds()))
                    e->takeDamage(50);
        }

        // Projectile hits enemy
        for (auto& e : enemies)
            if (e->isAlive() && player.consumeProjectileHit(e->getBounds()))
                e->takeDamage(25);

        // Dead regular enemies respawn at random position; boss stays dead
        for (auto& e : enemies) {
            if (!e->isAlive() && !e->isBoss()) {
                float rx = 100.f + static_cast<float>(std::rand() % static_cast<int>(worldW - 200.f));
                e->respawn({ rx, 50.f });
            }
        }

        // Enemy contact damages player
        for (auto& e : enemies) {
            if (e->isAlive() && player.getBounds().findIntersection(e->getBounds()))
                player.takeDamage(e->getContactDamage());
        }

        // Centre camera on player
        sf::FloatRect pb       = player.getBounds();
        sf::Vector2f  worldSize = world.getSize();
        float cx = pb.position.x + pb.size.x / 2.f;
        float cy = pb.position.y + pb.size.y / 2.f;
        cx = std::clamp(cx, viewSize.x / 2.f, worldSize.x - viewSize.x / 2.f);
        cy = std::clamp(cy, viewSize.y / 2.f, worldSize.y - viewSize.y / 2.f);
        camera.setCenter({ cx, cy });

        window.setView(camera);

        cycle.setView(camera);
        cycle.setWindowSize(viewSize);
        cycle.update(deltaTime);

        window.clear(cycle.getSkyColor());

        const sf::Sprite& sunRef  = cycle.getSunSprite();
        const sf::Sprite& moonRef = cycle.getMoonSprite();
        sf::Sprite sun  = sunRef;
        sf::Sprite moon = moonRef;
        sun.setColor(sf::Color(255, 255, 255, static_cast<unsigned char>(std::clamp(cycle.getDayFactor(), 0.f, 1.f) * 255.f)));
        moon.setColor(sf::Color(255, 255, 255, static_cast<unsigned char>(std::clamp(1.f - cycle.getDayFactor(), 0.f, 1.f) * 255.f)));
        window.draw(sun);
        window.draw(moon);
        world.Draw(window);
        for (auto& e : enemies)
            e->draw(window);
        for (auto& e : enemies)
            e->drawHealthBar(window);
        player.draw(window);

        // HUD in screen space
        window.setView(window.getDefaultView());
        player.drawHUD(window);

        // FPS counter — top-right corner
        int fps = (deltaTime > 0.f) ? static_cast<int>(1.f / deltaTime) : 0;
        fpsText.setString(std::to_string(fps) + " FPS");
        sf::FloatRect tb = fpsText.getLocalBounds();
        fpsText.setPosition({ viewSize.x - tb.size.x - 20.f, 15.f });
        window.draw(fpsText);

        window.display();
    }
}

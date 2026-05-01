#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "Enemies.hpp"
#include "Menu.hpp"
#include "Player.hpp"
#include "World.hpp"
#include "AssetManager.hpp"
#include "DayNightCycle.hpp"
#include "Music.hpp"
#include "Block.hpp"

enum GameState 
{ 
    MENU, 
    PLAYING, 
    PAUSED 
};

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 1280, 800 }), "Terraria 2");
    window.setFramerateLimit(60);

    Assets::loadTextures();
    Item::loadItems();

    DayNightCycle cycle(120.f);
    cycle.setSunTexture(&Assets::getTexture(3));
    cycle.setMoonTexture(&Assets::getTexture(4));

    World world = World();
    world.generateWorld(window);

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    const int   NUM_ENEMIES = 4;
    const float worldW      = world.getSize().x;
    std::vector<std::unique_ptr<Enemy>> enemies;
    enemies.reserve(NUM_ENEMIES + 1);
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

    // Synthesized mining click sound
    sf::SoundBuffer mineBuffer;
    {
        const unsigned sampleRate = 44100;
        const float    dur        = 0.07f;
        const auto     n          = static_cast<std::uint64_t>(sampleRate * dur);
        std::vector<std::int16_t> samples(n);
        for (std::uint64_t i = 0; i < n; ++i) {
            float progress = static_cast<float>(i) / static_cast<float>(n);
            float envelope = std::exp(-progress * 28.f);
            float freq     = 380.f - 180.f * progress;
            samples[i] = static_cast<std::int16_t>(
                std::sin(2.f * 3.14159265f * freq * static_cast<float>(i) / sampleRate)
                * envelope * 32767.f * 0.45f);
        }
        if (!mineBuffer.loadFromSamples(samples.data(), n, 1, sampleRate, {sf::SoundChannel::Mono}))
            std::cerr << "Failed to generate mine sound" << std::endl;
    }
    sf::Sound mineSound(mineBuffer);

    // Mining state
    int   mineRow          = -1;
    int   mineCol          = -1;
    float mineProgress     = 0.f;
    float mineTimer        = 0.f;
    const float mineInterval     = 0.25f;
    const int   mineHitsRequired = 3;
    const float mineRange        = static_cast<float>(TILE_SIZE) * 9.f;

    // Placement cooldown
    float placeCooldown    = 0.f;
    const float placeInterval = 0.15f;

    // Block type for each hotbar slot (dirt=1, grass=2)
    static constexpr int slotTypes[] = {1, 2};

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


    Menu mainMenu(1280.f, 800.f);
    mainMenu.setBackgroundImage("./Assets/Enemy/background.png");
    mainMenu.addItem("Start Game");
    mainMenu.addItem("Quit");

    Menu pauseMenu(1280.f, 800.f);
    pauseMenu.addItem("Resume");
    pauseMenu.addItem("Main Menu");
    pauseMenu.addItem("Quit");

    GameState gameState = MENU;  // Start in menu

    // Pause text
    sf::Text pauseText(font, "", 72);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setOutlineColor(sf::Color::Black);
    pauseText.setOutlineThickness(2.f);
    sf::FloatRect pauseBounds = pauseText.getLocalBounds();
    pauseText.setOrigin({ pauseBounds.size.x / 2.f, pauseBounds.size.y / 2.f });
    pauseText.setPosition({ viewSize.x / 2.f, viewSize.y / 2.f - 100.f });

    while (window.isOpen())
    {
        float deltaTime = deltaClock.restart().asSeconds();

        while (const std::optional event = window.pollEvent())
        {
            
            if (event->is<sf::Event::Closed>())
                window.close();

            if (auto* scroll = event->getIf<sf::Event::MouseWheelScrolled>()) {
                int dir = (scroll->delta > 0) ? -1 : 1;
                player.setSelectedSlot(player.getSelectedSlot() + dir);
            }

            // Menu input handling
            if (gameState == MENU)
            {
                mainMenu.handleInput(*event);
                
                if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>())
                {
                    if (keyEvent->code == sf::Keyboard::Key::Enter)
                    {
                        int selected = mainMenu.getSelectedItem();
                        if (selected == 0) // Start Game
                        {
                            gameState = PLAYING;
                            // Reset game state
                            player.setPosition({ worldCenterX, 50.f });
                            enemies.clear();
                            for (int i = 0; i < NUM_ENEMIES; ++i) {
                                auto e = std::make_unique<Enemy>("./Assets/Enemy/Enemy.png");
                                float rx = 100.f + static_cast<float>(std::rand() % static_cast<int>(worldW - 200.f));
                                e->setPosition({ rx, 50.f });
                                enemies.push_back(std::move(e));
                            }
                            bossTimer = 60.f;
                            bossSpawned = false;
                        }
                        else if (selected == 1) // Quit
                        {
                            window.close();
                        }
                    }
                }
            }
            // Pause menu input handling
            else if (gameState == PAUSED)
            {
                pauseMenu.handleInput(*event);
                
                if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>())
                {
                    if (keyEvent->code == sf::Keyboard::Key::Enter)
                    {
                        int selected = pauseMenu.getSelectedItem();
                        if (selected == 0) // Resume
                        {
                            gameState = PLAYING;
                        }
                        else if (selected == 1) // Main Menu
                        {
                            gameState = MENU;
                            // Reset game state for next play
                            player.setPosition({ worldCenterX, 50.f });
                            enemies.clear();
                            for (int i = 0; i < NUM_ENEMIES; ++i) {
                                auto e = std::make_unique<Enemy>("./Assets/Enemy/Enemy.png");
                                float rx = 100.f + static_cast<float>(std::rand() % static_cast<int>(worldW - 200.f));
                                e->setPosition({ rx, 50.f });
                                enemies.push_back(std::move(e));
                            }
                            bossTimer = 60.f;
                            bossSpawned = false;
                        }
                        else if (selected == 2) // Quit
                        {
                            window.close();
                        }
                    }
                    else if (keyEvent->code == sf::Keyboard::Key::P)
                    {
                        gameState = PLAYING; // Resume on P
                    }
                }
            }
        }

        // Only update game logic if playing
        if (gameState == PLAYING)
        {
            // Check for pause key
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P))
            {
                gameState = PAUSED;
            }

            for (auto& e : enemies)
                e->update(deltaTime, world);
            player.setAimPosition(window.mapPixelToCoords(sf::Mouse::getPosition(window), camera));
            player.update(deltaTime, world);

            if (placeCooldown > 0.f) placeCooldown -= deltaTime;

            sf::Vector2f mouseWorld = window.mapPixelToCoords(sf::Mouse::getPosition(window), camera);

            // Mining — left click, 3 hits to break a tile
            {
                sf::FloatRect pb      = player.getBounds();
                sf::Vector2f  pcenter = {pb.position.x + pb.size.x * 0.5f, pb.position.y + pb.size.y * 0.5f};
                int col = static_cast<int>(mouseWorld.x / TILE_SIZE);
                int row = static_cast<int>(mouseWorld.y / TILE_SIZE);
                float dx = mouseWorld.x - pcenter.x;
                float dy = mouseWorld.y - pcenter.y;
                float dist = std::sqrt(dx * dx + dy * dy);

                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) &&
                    world.isSolid(row, col) && dist <= mineRange)
                {
                    if (row != mineRow || col != mineCol) {
                        mineRow = row; mineCol = col;
                        mineProgress = 0.f; mineTimer = 0.f;
                    }
                    mineTimer += deltaTime;
                    if (mineTimer >= mineInterval) {
                        mineTimer -= mineInterval;
                        mineProgress += 1.f;
                        mineSound.play();
                        if (mineProgress >= static_cast<float>(mineHitsRequired)) {
                            int tileType = world.getTile(mineRow, mineCol);
                            world.setTile(mineRow, mineCol, 0);
                            player.addToInventory(tileType, 1);
                            mineRow = -1; mineCol = -1;
                            mineProgress = 0.f;
                        }
                    }
                } else if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    mineRow = -1; mineCol = -1;
                    mineProgress = 0.f; mineTimer = 0.f;
                }
            }

            // Block placement — right click
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) && placeCooldown <= 0.f) {
                int col = static_cast<int>(mouseWorld.x / TILE_SIZE);
                int row = static_cast<int>(mouseWorld.y / TILE_SIZE);
                if (!world.isSolid(row, col)) {
                    bool adj = world.isSolid(row - 1, col) || world.isSolid(row + 1, col) ||
                               world.isSolid(row, col - 1) || world.isSolid(row, col + 1);
                    if (adj) {
                        sf::FloatRect tileBounds({static_cast<float>(col * TILE_SIZE),
                                                  static_cast<float>(row * TILE_SIZE)},
                                                 {TILE_SIZE, TILE_SIZE});
                        if (!player.getBounds().findIntersection(tileBounds)) {
                            int tileType = slotTypes[player.getSelectedSlot()];
                            if (player.removeFromInventory(tileType, 1)) {
                                world.setTile(row, col, tileType);
                                placeCooldown = placeInterval;
                            }
                        }
                    }
                }
            }

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

            if (player.isSwordActive()) {
                sf::FloatRect sword = player.getSwordBounds();
                for (auto& e : enemies)
                    if (e->isAlive() && sword.findIntersection(e->getBounds()))
                        e->takeDamage(50);
            }

            for (auto& e : enemies)
                if (e->isAlive() && player.consumeProjectileHit(e->getBounds()))
                    e->takeDamage(25);

            for (auto& e : enemies) {
                if (!e->isAlive() && !e->isBoss()) {
                    float rx = 100.f + static_cast<float>(std::rand() % static_cast<int>(worldW - 200.f));
                    e->respawn({ rx, 50.f });
                }
            }

            for (auto& e : enemies) {
                if (e->isAlive() && player.getBounds().findIntersection(e->getBounds()))
                    player.takeDamage(e->getContactDamage());
            }
        }

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

        // Mining progress overlay (only when playing)
        if (gameState == PLAYING && mineRow >= 0 && mineCol >= 0) {
            float progress = mineProgress / static_cast<float>(mineHitsRequired);
            float tx = static_cast<float>(mineCol * TILE_SIZE);
            float ty = static_cast<float>(mineRow * TILE_SIZE);

            sf::RectangleShape overlay({TILE_SIZE, TILE_SIZE});
            overlay.setPosition({tx, ty});
            overlay.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(progress * 160)));
            window.draw(overlay);

            sf::RectangleShape barBg({TILE_SIZE, 3.f});
            barBg.setPosition({tx, ty + TILE_SIZE + 1.f});
            barBg.setFillColor(sf::Color(60, 60, 60, 200));
            window.draw(barBg);

            sf::RectangleShape barFill({TILE_SIZE * progress, 3.f});
            barFill.setPosition({tx, ty + TILE_SIZE + 1.f});
            barFill.setFillColor(sf::Color(255, 200, 50, 220));
            window.draw(barFill);
        }

        for (auto& e : enemies)
            e->draw(window);
        for (auto& e : enemies)
            e->drawHealthBar(window);
        player.draw(window);

        // Draw menu or pause overlay
        if (gameState == MENU)
        {
            window.setView(window.getDefaultView());
            mainMenu.draw(window);
            window.setView(camera);
        }
        else if (gameState == PAUSED)
        {
            window.setView(window.getDefaultView());
            pauseMenu.draw(window);
            // Draw "PAUSED" text
            window.draw(pauseText);
            window.setView(camera);
        }

        window.setView(window.getDefaultView());
        player.drawHUD(window);
        player.drawHotbar(window, font);

        int fps = (deltaTime > 0.f) ? static_cast<int>(1.f / deltaTime) : 0;
        fpsText.setString(std::to_string(fps) + " FPS");
        sf::FloatRect tb = fpsText.getLocalBounds();
        fpsText.setPosition({ viewSize.x - tb.size.x - 20.f, 15.f });
        window.draw(fpsText);

        window.display();
    }
}
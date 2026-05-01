#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
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
    DayNightCycle cycle(120.f); // 120 seconds per full cycle
    //assign textures
    cycle.setSunTexture(&Assets::getTexture(3));
    cycle.setMoonTexture(&Assets::getTexture(4));

    World world = World();
    world.generateWorld(window);

    Enemy enemy1("./Assets/Enemy/Enemy.jpg");
    Player player;
    float worldCenterX = world.getSize().x / 2.f;
    player.setPosition({ worldCenterX, 50.f });
    player.setSpawnPosition({ worldCenterX, 50.f });
    enemy1.setPosition({ worldCenterX + 200.f, 50.f });
    MusicPlayer music("./Assets/Music/theme.oga");
    music.play();

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

        enemy1.update(deltaTime, world);
        player.update(deltaTime, world);

        if (player.getBounds().findIntersection(enemy1.getBounds()))
            player.takeDamage(10);

        // Centre camera on player so it never shows outside the world
        sf::FloatRect pb = player.getBounds();
        sf::Vector2f   worldSize = world.getSize();
        float cx = pb.position.x + pb.size.x / 2.f;
        float cy = pb.position.y + pb.size.y / 2.f;
        cx = std::clamp(cx, viewSize.x / 2.f, worldSize.x - viewSize.x / 2.f);
        cy = std::clamp(cy, viewSize.y / 2.f, worldSize.y - viewSize.y / 2.f);
        camera.setCenter({ cx, cy });

        window.setView(camera);

        //update cycle with current camera/view
        cycle.setView(camera);
        cycle.setWindowSize(viewSize);
        cycle.update(deltaTime);

        window.clear(cycle.getSkyColor());

        //draw sun and moon behind the world
        const sf::Sprite& sunRef = cycle.getSunSprite();
        const sf::Sprite& moonRef = cycle.getMoonSprite();
        sf::Sprite sun = sunRef; //copy so we can modify color
        sf::Sprite moon = moonRef;
        sun.setColor(sf::Color(255,255,255, static_cast<unsigned char>(std::clamp(cycle.getDayFactor(), 0.f, 1.f) * 255.f)));
        moon.setColor(sf::Color(255,255,255, static_cast<unsigned char>(std::clamp(1.f - cycle.getDayFactor(), 0.f, 1.f) * 255.f)));
        window.draw(sun);
        window.draw(moon);
        world.Draw(window);
        enemy1.draw(window);
        player.draw(window);

        window.setView(window.getDefaultView());
        player.drawHUD(window);

        window.display();
    }
}

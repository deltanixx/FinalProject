#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "Enemies.hpp"
#include "Player.hpp"
#include "World.hpp"
#include "Music.hpp"
#include "Block.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 1280, 800 }), "SFML window");
    window.setFramerateLimit(60);

    Assets::loadTextures();
    Item::loadItems();

    World world = World();
    world.generateWorld(window);

    Enemy      enemy1("./Assets/Enemy/Enemy.jpg");
    Player     player;
    MusicPlayer music("./Assets/Music/theme.oga");
    music.play();

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

        window.clear(sf::Color(135, 206, 235));
        world.Draw(window);
        enemy1.draw(window);
        player.draw(window);
        window.display();
    }
}

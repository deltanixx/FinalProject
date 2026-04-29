#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Enemies.hpp"
#include "World.hpp"
#include "Music.hpp"
#include "Block.hpp"

int main()
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode({ 1280, 800 }), "SFML window");
    window.setFramerateLimit(60);
    Assets::loadTextures();
    Item::loadItems();

    World world = World();
    world.generateWorld(window);

    Enemy enemy1("./Assets/Enemy/Enemy.jpg"); //Enemy creation
    MusicPlayer music("./Assets/Music/theme.oga");
    music.play();

    sf::Clock deltaClock;  // tracks time between frames

    while (window.isOpen())
    {
        float deltaTime = deltaClock.restart().asSeconds(); //Time

        while (const std::optional event = window.pollEvent())
        {
            
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        enemy1.update(deltaTime, world);
        // Clear screen
        window.clear(sf::Color(135, 206, 235));

        /*
        *
        * Everything below from now on will need to be generated in the world class. It's more readable (:
        * 
        */
        world.Draw(window); 
        enemy1.draw(window); //Display enemy

        window.display();
    }
}
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Enemies.hpp"

int main()
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode({ 600, 800 }), "SFML window");

    Enemy enemy1("./Images/EnemyTest.jpg"); //Enemy creation

    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        while (const std::optional event = window.pollEvent())
        {
            // Close window: exit
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        enemy1.update(); //Move enemy
        // Clear screen
        window.clear(sf::Color::White);

        enemy1.draw(window); //Display enemy

        window.display();
    }
}
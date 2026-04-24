#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Enemies.hpp"

int main()
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode({ 600, 800 }), "SFML window");

    sf::Texture texture;

    //Template for enemy sprite & texture 
    texture.loadFromFile("./Images/Enemy.jpg");
    sf::Sprite sprite(texture);
    sprite.setScale(sf::Vector2f(0.1f, 0.15f));

    //sf::Font font("C:/Windows/Fonts/arial.ttf");
    //sf::Text text(font, "Hello SFML", 50);

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

        // Clear screen
        window.clear();

        window.draw(sprite);

        //window.draw(text);
        // 
        // Update the window
        window.display();
    }
}
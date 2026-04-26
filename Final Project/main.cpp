#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Enemies.hpp"
#include "World.hpp"
#include "Music.hpp"

int main()
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode({ 1280, 800 }), "SFML window");
    World world; 

    Enemy enemy1("./Assets/Enemy/EnemyTest.jpg"); //Enemy creation
    MusicPlayer music("./Assets/Music/theme.oga");  
    music.play();
    
    while (window.isOpen())
    {
        
        while (const std::optional event = window.pollEvent())
        {
            
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        enemy1.update(); //Movement for enemy
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
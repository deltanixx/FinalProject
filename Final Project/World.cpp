#include "World.hpp"
#include "Enemies.hpp"

World::World()
{
}

void World::Draw(sf::RenderWindow& window)
{
    const int tileSize = 16;
    sf::Texture dirtTexture;
    sf::Texture grassTexture; 
    dirtTexture.loadFromFile("./Assets/Tiles/dirt.png");
    grassTexture.loadFromFile("./Assets/Tiles/grass.png"); 

    sf::Sprite dirt(dirtTexture);
    sf::Sprite grass(grassTexture); //Could you try to make this more accessible? I can't really use collisions with the sprites being in a void function

    auto size = window.getSize(); 

    for (int col = 0; col <= (int)size.x / tileSize; col++)
    {
        grass.setPosition({ (float)(col * tileSize), size.y / 2.f });
        window.draw(grass);
    }

    for (int row = 1; row < (int)size.y / tileSize; row++)
    {
        for (int col = 0; col <= (int)size.x / tileSize; col++)
        {
            dirt.setPosition({ (float)(col * tileSize), size.y / 2.f + (float)(row * tileSize) });
            window.draw(dirt);
        }
    }
}

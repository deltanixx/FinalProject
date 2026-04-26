#include "World.hpp"

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
    sf::Sprite grass(grassTexture);

    for (int col = 0; col <= 1280 / tileSize; col++)
    {
        grass.setPosition({ (float)(col * tileSize), 400.f});
        window.draw(grass);
    }

    for (int row = 1; row < 400 / tileSize; row++)
    {
        for (int col = 0; col <= 1280 / tileSize; col++)
        {
            dirt.setPosition({ (float)(col * tileSize), 400.f + (float)(row * tileSize) });
            window.draw(dirt);
        }
    }
}
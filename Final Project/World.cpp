#include "World.hpp"
#include "Enemies.hpp"
#include "AssetManager.hpp"

World::World()
{
    worldMatrix.resize(800 / TILE_SIZE);
    for (int i = 0; i < 800 / TILE_SIZE; i++)
        worldMatrix[i].resize(1280 / TILE_SIZE);
}

void World::Draw(sf::RenderWindow& window)
{
    auto size = window.getSize(); 
    const int tileSize = 16;
    
    //sf::Texture dirtTexture;
    //sf::Texture grassTexture; 
    //dirtTexture.loadFromFile("./Assets/Tiles/dirt.png");
    //grassTexture.loadFromFile("./Assets/Tiles/grass.png"); 

   /* sf::Sprite dirt(Assets::getTexture(1));
    sf::Sprite grass(Assets::getTexture(2));*/ //Could you try to make this more accessible? I can't really use collisions with the sprites being in a void function


    for (int i = 0; i < worldMatrix.size(); i++)
    {
        for (int j = 0; j < worldMatrix[0].size(); j++)
        {
            sf::Sprite block = dynamic_cast<Block*>(Item::getItem(worldMatrix[i][j]))->getSprite();
            block.setPosition({(float)(j * TILE_SIZE), (float)(i * TILE_SIZE)});
            window.draw(block);
        }
    }
        
    //for (int col = 0; col <= (int)size.x / tileSize; col++)
    //{
    //    grass.setPosition({ (float)(col * tileSize), size.y / 2.f });
    //    window.draw(grass);
    //}

    //for (int row = 1; row < (int)size.y / tileSize; row++)
    //{
    //    for (int col = 0; col <= (int)size.x / tileSize; col++)
    //    {
    //        dirt.setPosition({ (float)(col * tileSize), size.y / 2.f + (float)(row * tileSize) });
    //        window.draw(dirt);
    //    }
    //}
}

void World::generateWorld(sf::RenderWindow& window) 
{
    auto size = window.getSize();
    for (int i = 0; i < worldMatrix.size(); i++) 
    {
        for (int j = 0; j < worldMatrix[0].size(); j++) 
        {
            worldMatrix[i][j] = 0;
        }
    }

    for (int j = 0; j < worldMatrix[0].size(); j++) 
    {
        worldMatrix[worldMatrix.size() / 2][j] = 2;
    }

    for (int i = (worldMatrix.size() / 2) + 1; i < worldMatrix.size(); i++) 
    {
        for (int j = 0; j < worldMatrix[0].size(); j++) 
        {
            worldMatrix[i][j] = 1;
        }
    }

    std::cout << "World Generated" << std::endl;
}

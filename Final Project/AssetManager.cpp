#include "AssetManager.hpp"

std::vector<sf::Texture> Assets::tiles;

void Assets::loadTextures() 
{
	tiles.push_back(sf::Texture(sf::Image({16, 16}, sf::Color::Transparent)));
	tiles.push_back(sf::Texture("Assets/Tiles/dirt.png"));
	tiles.push_back(sf::Texture("Assets/Tiles/grass.png"));

	std::cout << "Textures Loaded" << std::endl;
}

sf::Texture& Assets::getTexture(int id) 
{
	return tiles[id];
}
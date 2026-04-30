#include "AssetManager.hpp"

std::vector<sf::Texture> Assets::tiles;

void Assets::loadTextures() 
{
  tiles.clear();

    tiles.emplace_back();


	{
		sf::Texture t;
		if (!t.loadFromFile("Assets/Tiles/dirt.png")) std::cerr << "Failed to load Assets/Tiles/dirt.png\n";
		tiles.push_back(std::move(t));
	}


	{
		sf::Texture t;
		if (!t.loadFromFile("Assets/Tiles/grass.png")) std::cerr << "Failed to load Assets/Tiles/grass.png\n";
		tiles.push_back(std::move(t));
	}


	{
		sf::Texture t;
		if (!t.loadFromFile("Assets/DayNight/Andy Sun.png")) std::cerr << "Failed to load Assets/DayNight/Andy Sun.png\n";
		tiles.push_back(std::move(t));
	}


	{
		sf::Texture t;
		if (!t.loadFromFile("Assets/DayNight/Moon Image.png")) std::cerr << "Failed to load Assets/DayNight/Moon Image.png\n";
		tiles.push_back(std::move(t));
	}

	std::cout << "Textures Loaded: " << tiles.size() << std::endl;
}

sf::Texture& Assets::getTexture(int id) 
{
	return tiles[id];
}

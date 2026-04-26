#pragma once
#include <SFML/Graphics.hpp>

class Tile
{
public:
	void setTileSprite(sf::Texture& tex); 
	sf::Sprite getSprite(); 

	void setPosition(sf::Vector2<float> pos);

private:
	sf::Sprite tileSprite;
};
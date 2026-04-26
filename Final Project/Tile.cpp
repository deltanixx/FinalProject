#include "Tile.hpp"

void Tile::setTileSprite(sf::Texture& tex)
{
	this->tileSprite.setTexture(tex); 
}

sf::Sprite Tile::getSprite()
{
	return this->tileSprite; 
}

void Tile::setPosition(sf::Vector2<float> pos)
{
	this->tileSprite.setPosition(pos); 
}
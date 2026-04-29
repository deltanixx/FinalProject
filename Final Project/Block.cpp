#include "Block.hpp"

Block::Block(const std::string& name, const int& id, const int& textureId) : Item(name, id, textureId), sprite(sf::Sprite(*texture))
{
}

Block::Block(Block& rhs) : Item(rhs), sprite(rhs.sprite) 
{
}

Block& Block::operator=(Block& rhs) 
{
	this->name = rhs.name;
	this->id = rhs.id;
	this->texture = rhs.texture;
	this->sprite = rhs.sprite;

	return *this;
}

Block::~Block() 
{
}

void Block::use() 
{

}

sf::Sprite Block::getSprite()
{
	return this->sprite;
}

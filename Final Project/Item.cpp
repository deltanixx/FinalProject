#include "Item.hpp"

Item::Item(const std::string& name, const int& id, const std::string& textureFileName) 
{
	this->name = name;
	this->id = id;
	this->texture.loadFromFile(textureFileName);
}

Item::Item(const Item& rhs) 
{
	this->name = rhs.name;
	this->id = rhs.id;
	this->texture = rhs.texture;
}

Item& Item::operator=(Item& rhs) 
{
	this->name = rhs.name;
	this->id = rhs.id;
	this->texture = rhs.texture;

	return *this;
}
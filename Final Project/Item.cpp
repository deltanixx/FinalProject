#include "Item.hpp"
#include "Block.hpp"

std::vector<Item*> Item::itemList;

Item::Item(const std::string& name, const int& id, const int& textureId) : texture(&Assets::getTexture(textureId))
{
	this->name = name;
	this->id = id;
}

Item::Item(const Item& rhs) : texture(rhs.texture)
{
	this->name = rhs.name;
	this->id = rhs.id;
}

Item& Item::operator=(Item& rhs) 
{
	this->name = rhs.name;
	this->id = rhs.id;
	this->texture = rhs.texture;

	return *this;
}

Item::~Item() 
{

}

void Item::use() {}

std::string Item::getName() const 
{
	return this->name;
}

int Item::getId() const 
{
	return this->id;
}

sf::Texture Item::getTexture() const 
{
	return *this->texture;
}

void Item::loadItems() 
{
	itemList.push_back(new Block("Air", 0, 0));
	itemList.push_back(new Block("Dirt", 1, 1));
	itemList.push_back(new Block("Grass", 2, 2));

	std::cout << "Items Loaded" << std::endl;
}

Item* Item::getItem(const int& id) 
{
	return itemList[id];
}

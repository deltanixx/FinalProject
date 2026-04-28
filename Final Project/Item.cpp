#include "Item.hpp"

std::vector<Item> Item::itemList;

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

std::string Item::getName() const {
	return this->name;
}

int Item::getId() const {
	return this->id;
}

sf::Texture Item::getTexture() const {
	return this->texture;
}

void Item::loadItems() {
	itemList.push_back(Item("TestItem1", 0, "Assets/Tiles/dirt.png"));

	std::cout << "Items Loaded" << std::endl;
}

Item& Item::getItem(const int& id) {
	return itemList[id];
}

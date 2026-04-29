#pragma once

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "AssetManager.hpp"

class Item 
{
public:
	Item(const std::string& name, const int& id, const int& textureId);

	Item(const Item& rhs);

	Item& operator=(Item& rhs);

	~Item();

	virtual void use();

	// getters
	std::string getName() const;
	int getId() const;
	sf::Texture getTexture() const;


	static void loadItems(); // initialize items
	static Item* getItem(const int& id);

protected:
	std::string name;
	int id;
	sf::Texture* texture;

private:
	static std::vector<Item*> itemList;
};
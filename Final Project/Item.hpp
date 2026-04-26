#pragma once

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>

class Item 
{
public:
	Item(const std::string& name, const int& id, const std::string& textureFileName);

	Item(const Item& rhs);

	Item& operator=(Item& rhs);

protected:
	std::string name;
	int id;
	sf::Texture texture;
};
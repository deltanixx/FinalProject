#pragma once

#include "Item.hpp"

class Block : public Item {
public:
	Block(const std::string& name, const int& id, const int& textureId);

	Block(Block& rhs);

	Block& operator=(Block& rhs);

	~Block();

	void use() override;

	sf::Sprite getSprite();

private:
	sf::Sprite sprite;
};
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Block.hpp"

// not used for now
#define WORLD_SIZE_HORIZONTAL 1000
#define WORLD_SIZE_VERTICAL 1000
#define TILE_SIZE 16


class World
{
public: 
	World(); 
	void Draw(sf::RenderWindow& window);
	void generateWorld(sf::RenderWindow& window);

private:
	std::vector<std::vector<int>> worldMatrix;
};
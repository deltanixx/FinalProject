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

	/// Returns true if the tile at (row, col) is solid. Out-of-bounds tiles are treated as air.
	bool isSolid(int row, int col) const;

	/// Moves pos by vel*dt on each axis separately and pushes the entity out of any solid tiles.
	/// Sets onGround=true when resting on a surface, zeroes the relevant velocity component on impact.
	void resolveCollision(sf::Vector2f& pos, const sf::Vector2f& size, sf::Vector2f& vel, bool& onGround, float deltaTime) const;

private:
	std::vector<std::vector<int>> worldMatrix;
	std::vector<sf::Sprite> spriteCache; // pre-built, positioned sprites — rebuilt in generateWorld
};
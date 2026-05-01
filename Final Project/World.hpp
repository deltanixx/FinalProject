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

	/// Returns the world dimensions in pixels.
	sf::Vector2f getSize() const;

	/// Returns true if the tile at (row, col) is solid. Out-of-bounds tiles are treated as air.
	bool isSolid(int row, int col) const;

	/// Returns the tile type at (row, col), or 0 for out-of-bounds.
	int  getTile(int row, int col) const;

	/// Sets the tile at (row, col) to type and rebuilds the sprite cache.
	void setTile(int row, int col, int type);

	/// Moves pos by vel*dt on each axis separately and pushes the entity out of any solid tiles.
	/// Sets onGround=true when resting on a surface, zeroes the relevant velocity component on impact.
	void resolveCollision(sf::Vector2f& pos, const sf::Vector2f& size, sf::Vector2f& vel, bool& onGround, float deltaTime) const;

private:
	std::vector<std::vector<int>> worldMatrix;
	std::vector<sf::Sprite> spriteCache;
	std::vector<int> surfaceRows_;
	void buildSpriteCache();
};
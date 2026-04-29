#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

class Assets {
public:

	static void loadTextures();
	static sf::Texture& getTexture(int id);

private:
	static std::vector<sf::Texture> tiles;
};


#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Menu {
private:
    sf::Font font;
    std::vector<sf::Text> menuItems;
    int selectedIndex;
    sf::Vector2f windowSize;

public:
    Menu(float windowWidth, float windowHeight);

    void addItem(const std::string& text);
    void moveUp();
    void moveDown();
    int getSelectedItem() const;
    void draw(sf::RenderWindow& window);
    void handleInput(const sf::Event& event);
};
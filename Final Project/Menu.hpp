#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>
#include <string>

class Menu {
private:
    sf::Font font;
    sf::Texture m_backgroundTexture;  
    std::optional<sf::Sprite> m_backgroundSprite;
    std::vector<sf::Text> menuItems;
    int selectedIndex;
    sf::Vector2f windowSize;
    bool hasBackground;

public:
    Menu(float windowWidth, float windowHeight);

    void setBackgroundImage(const std::string& imagePath);

    void addItem(const std::string& text);
    void moveUp();
    void moveDown();
    int getSelectedItem() const;
    void draw(sf::RenderWindow& window);
    void handleInput(const sf::Event& event);
};
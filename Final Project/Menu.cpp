#include "Menu.hpp"
#include <iostream>

Menu::Menu(float windowWidth, float windowHeight) : selectedIndex(0), windowSize(windowWidth, windowHeight)
{
    // Load font
    if (!font.openFromFile("./Assets/Fonts/PixelText.ttf")) {
        std::cerr << "Font failed" << std::endl;
    }
}

void Menu::addItem(const std::string& text)
{
    sf::Text menuItem(font);
    menuItem.setFont(font);
    menuItem.setString(text);
    menuItem.setCharacterSize(48);
    menuItem.setFillColor(sf::Color::White);

    // Center text 
    sf::FloatRect textBounds = menuItem.getLocalBounds();
    menuItem.setOrigin({ textBounds.size.x / 2.f, textBounds.size.y / 2.f });
    menuItem.setPosition({ windowSize.x / 2.f, 200.f + menuItems.size() * 80.f });

    menuItems.push_back(menuItem);

    // Highlight selected text
    if (menuItems.size() == 1) {
        menuItems[0].setFillColor(sf::Color::Yellow);
    }
}

void Menu::moveUp()
{
    if (selectedIndex > 0) {
        // Reset current item color
        menuItems[selectedIndex].setFillColor(sf::Color::White);
        selectedIndex--;
        // Highlight new item
        menuItems[selectedIndex].setFillColor(sf::Color::Yellow);
    }
}

void Menu::moveDown()
{
    if (selectedIndex < menuItems.size() - 1) {
        menuItems[selectedIndex].setFillColor(sf::Color::White);
        selectedIndex++;
        menuItems[selectedIndex].setFillColor(sf::Color::Yellow);
    }
}

int Menu::getSelectedItem() const
{
    return selectedIndex;
}

void Menu::draw(sf::RenderWindow& window)
{
    // Draw background (optional)
    sf::RectangleShape background(sf::Vector2f(windowSize.x, windowSize.y));
    background.setFillColor(sf::Color(50, 50, 50, 200));  // Semi-transparent dark
    window.draw(background);

    // Draw title
    sf::Text title(font);
    title.setFont(font);
    title.setString("Pixel Game");
    title.setCharacterSize(72);
    title.setFillColor(sf::Color::Green);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin({ titleBounds.size.x / 2.f, titleBounds.size.y / 2.f });
    title.setPosition({ windowSize.x / 2.f, 100.f });
    window.draw(title);

    // Draw menu items
    for (auto& item : menuItems) {
        window.draw(item);
    }
}

void Menu::handleInput(const sf::Event& event)
{
    if (event.is<sf::Event::KeyPressed>()) {
        auto keyEvent = event.getIf<sf::Event::KeyPressed>();

        if (keyEvent->code == sf::Keyboard::Key::Up) {
            moveUp();
        }
        else if (keyEvent->code == sf::Keyboard::Key::Down) {
            moveDown();
        }
        else if (keyEvent->code == sf::Keyboard::Key::Enter) {
            // Selection will be handled in main
        }
    }
}
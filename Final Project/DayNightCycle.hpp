#pragma once
#include <SFML/Graphics.hpp>

class DayNightCycle {
public:
    DayNightCycle(float daySeconds = 60.f);
    void update(float dt);

    // Sky color to clear the window with
    sf::Color getSkyColor() const;

    // Sun and moon sprites (returned by value)
    sf::Sprite getSunSprite() const;
    sf::Sprite getMoonSprite() const;

    // Whether sun or moon should be visible (draw both but alpha controlled)
    float getDayFactor() const { return dayFactor; } // 1.0 = day, 0.0 = night

    // Set the view (camera) and window size so world-to-screen positions work
    void setView(const sf::View& v) { view = v; }
    void setWindowSize(const sf::Vector2f& s) { windowSize = s; }

    void setSunTexture(const sf::Texture* t) { sunTexture = t; }
    void setMoonTexture(const sf::Texture* t) { moonTexture = t; }

private:
    float time = 0.f;
    float dayLength = 60.f;
    float dayFactor = 1.f;

    const sf::Texture* sunTexture = nullptr;
    const sf::Texture* moonTexture = nullptr;
    sf::Vector2f sunPos;
    sf::Vector2f moonPos;

    float fadeDuration = 1.0f; // seconds for quick transition
    float fadeTimer = 0.f;
    float targetDayFactor = 1.f;
    float startDayFactor = 1.f;

    sf::View view;
    sf::Vector2f windowSize = {1280.f, 800.f};
};

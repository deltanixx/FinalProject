#include "DayNightCycle.hpp"
#include "AssetManager.hpp"
#include <cmath>

DayNightCycle::DayNightCycle(float daySeconds)
{
    dayLength = daySeconds;
}

void DayNightCycle::update(float dt)
{
    time += dt;
    while (time >= dayLength) time -= dayLength;

    // normalized t [0,1)
    float t = time / dayLength;


    float moveAngle = t * 2.f * 3.14159265f;

    //world/space positions relative to camera
    sf::Vector2f center = view.getCenter();
    sf::Vector2f size = windowSize;

    //neither are 0.5 for better looks
    float rx = size.x * 0.6f;
    float ry = size.y * 0.45f;

    // sun moves with angle when t=0.25 (midday) sun at top
    float sx = center.x + rx * std::cos(moveAngle - 3.14159265f/2.f);
    float sy = center.y + ry * std::sin(moveAngle - 3.14159265f/2.f);

    float mx = center.x + rx * std::cos(moveAngle - 3.14159265f/2.f + 3.14159265f);
    float my = center.y + ry * std::sin(moveAngle - 3.14159265f/2.f + 3.14159265f);

	//save positions for sprite drawing
    sunPos = { sx, sy };
    moonPos = { mx, my };


    bool sunAbove = (sunPos.y < view.getCenter().y);
    bool moonAbove = (moonPos.y < view.getCenter().y);

    //determine immediate target
    if (sunAbove && !moonAbove)
        targetDayFactor = 1.f;
    else if (moonAbove && !sunAbove)
        targetDayFactor = 0.f;
    else
    {
       
        float elev = (view.getCenter().y - sunPos.y) / (windowSize.y * 0.5f);
        targetDayFactor = std::clamp(elev, 0.f, 1.f);
    }

    //if target changed, start fade
    if (std::abs(targetDayFactor - dayFactor) > 0.001f)
    {
        startDayFactor = dayFactor;
        fadeTimer = 0.f;
    }

    //advance fade
    if (fadeTimer < fadeDuration)
    {
        fadeTimer += dt;
        float u = std::clamp(fadeTimer / fadeDuration, 0.f, 1.f);
        //smooth in/out for better look
        float ease = u * u * (3.f - 2.f * u);
        dayFactor = startDayFactor + (targetDayFactor - startDayFactor) * ease;
    }

}

sf::Sprite DayNightCycle::getSunSprite() const
{
    const sf::Texture& tex = sunTexture ? *sunTexture : Assets::getTexture(0);
    sf::Sprite s(tex);
    auto lb = s.getLocalBounds();
    s.setOrigin({ lb.size.x * 0.5f, lb.size.y * 0.5f });
    s.setPosition(sunPos);
    return s;
}

sf::Sprite DayNightCycle::getMoonSprite() const
{
    const sf::Texture& tex = moonTexture ? *moonTexture : Assets::getTexture(0);
    sf::Sprite m(tex);
    auto lb = m.getLocalBounds();
    m.setOrigin({ lb.size.x * 0.5f, lb.size.y * 0.5f });
    m.setPosition(moonPos);
    return m;
}

//sprites returned by reference

sf::Color DayNightCycle::getSkyColor() const
{
    //day and night are light and dark blue
    sf::Color day(135, 206, 235);
    sf::Color night(10, 12, 40);

    auto mix = [](const sf::Color& a, const sf::Color& b, float t){
        auto mixChannel = [&](unsigned char ca, unsigned char cb){
            return static_cast<unsigned char>(ca * t + cb * (1.f - t));
        };
        return sf::Color(

            mixChannel(a.r, b.r),
            mixChannel(a.g, b.g),
            mixChannel(a.b, b.b)

        );
    };

    //dayFactor==1 => day, 0 => night
    return mix(day, night, dayFactor);
}

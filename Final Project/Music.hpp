#pragma once
#include <SFML/Audio.hpp>
#include <string>

class MusicPlayer {
public:
    MusicPlayer(const std::string& filePath);
    void play();
    void pause();
    void stop();
    void setVolume(float volume);  // 0 to 100

private:
    sf::Music music;
};
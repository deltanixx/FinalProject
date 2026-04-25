#include "Music.hpp"
#include <iostream>

MusicPlayer::MusicPlayer(const std::string& filePath)
{
    if (!music.openFromFile(filePath)) {
        std::cerr << "Music failed" << std::endl;
    }
    else {
        std::cout << "Music loaded" << std::endl;
        music.setLooping(true);   // loop the music
        music.setVolume(50.f);    // 50% volume
    }
}

void MusicPlayer::play()
{
    music.play();
}

void MusicPlayer::pause()
{
    music.pause();
}

void MusicPlayer::stop()
{
    music.stop();
}

void MusicPlayer::setVolume(float volume)
{
    music.setVolume(volume);
}
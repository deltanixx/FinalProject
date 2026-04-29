#include "World.hpp"
#include "Enemies.hpp"
#include "AssetManager.hpp"
#include <random>
#include <numeric>
#include <algorithm>
#include <cmath>

static std::vector<int> perm;

/// Initializes the Perlin noise permutation table with the given seed.
static void initNoise(unsigned int seed)
{
    perm.resize(512);
    std::iota(perm.begin(), perm.begin() + 256, 0);
    std::shuffle(perm.begin(), perm.begin() + 256, std::default_random_engine(seed));
    for (int i = 0; i < 256; i++)
        perm[256 + i] = perm[i];
}

static float fade(float t) { return t * t * t * (t * (t * 6.f - 15.f) + 10.f); }
static float lerp(float a, float b, float t) { return a + t * (b - a); }
static float grad(int hash, float x) { return (hash & 1) ? x : -x; }

/// Returns a single-octave Perlin noise value in [-1, 1] for position x.
static float noise1D(float x)
{
    int X = (int)std::floor(x) & 255;
    x -= std::floor(x);
    return lerp(grad(perm[X], x), grad(perm[X + 1], x - 1.f), fade(x));
}

/// Stacks multiple noise octaves (each half the amplitude of the last) for natural-looking terrain.
static float octaveNoise(float x, int octaves, float persistence)
{
    float total = 0.f, amplitude = 1.f, frequency = 1.f, maxValue = 0.f;
    for (int i = 0; i < octaves; i++)
    {
        total     += noise1D(x * frequency) * amplitude;
        maxValue  += amplitude;
        amplitude *= persistence;
        frequency *= 2.f;
    }
    return total / maxValue;
}

World::World()
{
    worldMatrix.resize(800 / TILE_SIZE);
    for (int i = 0; i < 800 / TILE_SIZE; i++)
        worldMatrix[i].resize(1280 / TILE_SIZE);
}

/// Draws every tile in the world matrix at its corresponding screen position.
void World::Draw(sf::RenderWindow& window)
{
    for (int i = 0; i < worldMatrix.size(); i++)
    {
        for (int j = 0; j < worldMatrix[0].size(); j++)
        {
            sf::Sprite block = dynamic_cast<Block*>(Item::getItem(worldMatrix[i][j]))->getSprite();
            block.setPosition({(float)(j * TILE_SIZE), (float)(i * TILE_SIZE)});
            window.draw(block);
        }
    }
}

/// Procedurally generates terrain using layered Perlin noise, placing grass at the surface and dirt below.
void World::generateWorld(sf::RenderWindow& window)
{
    const int rows = (int)worldMatrix.size();
    const int cols = (int)worldMatrix[0].size();
    const int midRow = rows / 2;
    const int amplitude = rows / 5;

    initNoise(std::random_device{}());

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            worldMatrix[i][j] = 0;

    for (int j = 0; j < cols; j++)
    {
        float nx = (float)j / cols * 3.f;
        float n = octaveNoise(nx, 4, 0.5f);
        int surfaceRow = std::clamp(midRow + (int)(n * amplitude), 1, rows - 2);

        worldMatrix[surfaceRow][j] = 2;
        for (int i = surfaceRow + 1; i < rows; i++)
            worldMatrix[i][j] = 1;
    }

    std::cout << "World Generated" << std::endl;
}
        
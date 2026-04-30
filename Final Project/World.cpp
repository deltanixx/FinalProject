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
    const int rows = 800 / TILE_SIZE;
    const int cols = (1280 * 4) / TILE_SIZE; // 4x the window width for side-scrolling
    worldMatrix.resize(rows);
    for (int i = 0; i < rows; i++)
        worldMatrix[i].resize(cols);
}

sf::Vector2f World::getSize() const
{
    return sf::Vector2f(
        (float)(worldMatrix[0].size() * TILE_SIZE),
        (float)(worldMatrix.size()    * TILE_SIZE)
    );
}

/// Draws the pre-built sprite cache — no lookups or allocations per frame.
void World::Draw(sf::RenderWindow& window)
{
    for (const sf::Sprite& sprite : spriteCache)
        window.draw(sprite);
}

/// Procedurally generates terrain using layered Perlin noise, placing grass at the surface and dirt below.
void World::generateWorld(sf::RenderWindow& window)
{
    const int rows = (int)worldMatrix.size();
    const int cols = (int)worldMatrix[0].size();
    const int midRow = rows / 2;
    const int amplitude = rows / 3;

    initNoise(std::random_device{}());

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            worldMatrix[i][j] = 0;

    std::vector<int> surfaceRows(cols);
    for (int j = 0; j < cols; j++)
    {
        float nx = (float)j / cols * 10.f;
        float n = octaveNoise(nx, 6, 0.55f);
        int surfaceRow = std::clamp(midRow + (int)(n * amplitude), 1, rows - 2);
        surfaceRows[j] = surfaceRow;

        worldMatrix[surfaceRow][j] = 2;
        for (int i = surfaceRow + 1; i < rows; i++)
            worldMatrix[i][j] = 1;
    }

    // Build sprite cache — done once here so Draw has zero per-frame overhead
    spriteCache.clear();
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (worldMatrix[i][j] == 0) continue; // skip air tiles
            sf::Sprite sprite = dynamic_cast<Block*>(Item::getItem(worldMatrix[i][j]))->getSprite();
            sprite.setPosition({ (float)(j * TILE_SIZE), (float)(i * TILE_SIZE) });

            // darken dirt tiles based on how far below the surface they are
            if (worldMatrix[i][j] == 1)
            {
                int depth = i - surfaceRows[j];
                uint8_t brightness = (uint8_t)std::max(20, 255 - depth * 25);
                sprite.setColor(sf::Color(brightness, brightness, brightness));
            }

            spriteCache.push_back(sprite);
        }
    }

    std::cout << "World Generated" << std::endl;
}

bool World::isSolid(int row, int col) const
{
    if (row < 0 || row >= (int)worldMatrix.size())    return false;
    if (col < 0 || col >= (int)worldMatrix[0].size()) return false;
    return worldMatrix[row][col] != 0;
}

void World::resolveCollision(sf::Vector2f& pos, const sf::Vector2f& size, sf::Vector2f& vel, bool& onGround, float deltaTime) const
{
    auto tileRow = [](float y) { return (int)(y / TILE_SIZE); };
    auto tileCol = [](float x) { return (int)(x / TILE_SIZE); };

    // Resolve X axis first — move horizontally, stop at the first solid tile hit
    pos.x += vel.x * deltaTime;
    {
        int rowTop    = tileRow(pos.y + 0.001f);          // slight inset so we don't grab the ceiling tile
        int rowBottom = tileRow(pos.y + size.y - 0.001f); // exclude the floor tile the entity rests on
        int colLeft   = tileCol(pos.x);
        int colRight  = tileCol(pos.x + size.x);

        bool hitX = false;
        for (int row = rowTop; row <= rowBottom && !hitX; row++)
            for (int col = colLeft; col <= colRight && !hitX; col++)
                if (isSolid(row, col))
                {
                    pos.x = (vel.x > 0) ? (float)(col * TILE_SIZE) - size.x
                                        : (float)((col + 1) * TILE_SIZE);
                    vel.x = 0.f;
                    hitX  = true;
                }
    }

    // Resolve Y axis — scan in the direction of travel so we always hit the first surface, not the deepest
    onGround = false;
    pos.y += vel.y * deltaTime;
    {
        int rowTop    = tileRow(pos.y);
        int rowBottom = tileRow(pos.y + size.y);
        int colLeft   = tileCol(pos.x);
        int colRight  = tileCol(pos.x + size.x);

        bool hitY = false;
        if (vel.y >= 0)  // falling — scan top-down, land on the shallowest solid tile
        {
            for (int row = rowTop; row <= rowBottom && !hitY; row++)
                for (int col = colLeft; col <= colRight && !hitY; col++)
                    if (isSolid(row, col))
                    {
                        pos.y    = (float)(row * TILE_SIZE) - size.y;
                        onGround = true;
                        vel.y    = 0.f;
                        hitY     = true;
                    }
        }
        else  // rising — scan bottom-up, stop at the deepest solid tile overhead
        {
            for (int row = rowBottom; row >= rowTop && !hitY; row--)
                for (int col = colLeft; col <= colRight && !hitY; col++)
                    if (isSolid(row, col))
                    {
                        pos.y = (float)((row + 1) * TILE_SIZE);
                        vel.y = 0.f;
                        hitY  = true;
                    }
        }
    }
}
        
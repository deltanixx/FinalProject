# Final Project — Terraria-Inspired Game

A 2D sandbox survival game built in C++ with SFML 3, inspired by Terraria. Mine terrain, fight enemies, survive a boss, and build structures in a procedurally generated world.

## Demo

https://github.com/deltanixx/FinalProject/raw/main/demo.mp4

## Features

- Procedurally generated tile-based world using layered Perlin noise
- Mining system with hit-progress and inventory tracking
- Block placement with adjacency validation
- Enemy AI with patrol, chase, and respawn logic
- Timed boss spawn with unique stats and purple health bar
- Projectile combat (sword throw) and melee swing
- Dynamic day/night cycle with sky color blending and celestial body animation
- Player animation, damage cooldown, and respawn on death
- Synthesized audio — no external SFX files for mining or sword swing
- Hotbar, health bar HUD, and kill counter
- Main menu, pause menu, and in-game music

## Requirements

- Windows 10 or later
- [Visual Studio 2022](https://visualstudio.microsoft.com/) with the **Desktop development with C++** workload

No additional dependencies — SFML 3 is bundled in the repo (`include/` and `lib/`).

## Building

1. Clone the repository
   ```
   git clone https://github.com/deltanixx/FinalProject.git
   ```
2. Open `Final Project.slnx` in Visual Studio 2022
3. Set the configuration to **Debug | x64**
4. Press **Ctrl+Shift+B** to build
5. Press **F5** to run

The SFML DLLs are already placed in the output directories so the game runs immediately after building.

## Controls

| Action | Input |
|---|---|
| Move | A / D or Arrow Keys |
| Jump | Space / W / Up Arrow |
| Mine tile | Hold Left Click |
| Place block | Right Click |
| Swing sword | Z |
| Scroll hotbar | Mouse Wheel |
| Pause | P |
| Menu navigation | Arrow Keys + Enter |

## Project Structure

```
Final Project/          — Visual Studio project, source files, images & music
include/SFML/           — SFML 3 headers
lib/                    — SFML 3 import and static libraries
x64/Debug/              — Debug build output (DLLs included)
x64/Release/            — Release build output (DLLs included)
```

---

## Implementation Write-Up

### Procedural Terrain Generation

The world is generated using layered 1D Perlin noise (also called octave/fractal noise). The raw `noise1D` function returns a smooth value in `[-1, 1]` for a given x coordinate. `octaveNoise` stacks multiple passes of that, each at double the frequency and half the amplitude, producing natural-looking hills rather than perfectly smooth curves.

```cpp
// World.cpp
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
```

During world generation each column gets a surface row from `octaveNoise`, the surface tile is set to grass (type 2), and everything below it is filled with dirt (type 1). The world is also 4x the window width to allow side-scrolling.

```cpp
for (int j = 0; j < cols; j++)
{
    float nx = (float)j / cols * 10.f;
    float n  = octaveNoise(nx, 6, 0.55f);
    int surfaceRow = std::clamp(midRow + (int)(n * amplitude), 1, rows - 2);

    worldMatrix[surfaceRow][j] = 2; // grass
    for (int i = surfaceRow + 1; i < rows; i++)
        worldMatrix[i][j] = 1;     // dirt
}
```

A depth-based brightness pass then darkens dirt tiles the further they are below the surface, giving the impression of underground depth without a separate lighting system.

---

### Physics & Tile Collision

Every entity (player and enemies) inherits from `PhysicsEntity`, which applies gravity each frame and delegates collision resolution to the world. Collision is resolved on each axis separately — X first, then Y — to avoid corner-clipping artifacts.

```cpp
// PhysicsEntity.cpp
void PhysicsEntity::applyPhysics(float deltaTime, const World& world)
{
    if (!onGround)
        velocity.y += gravity * deltaTime;
    world.resolveCollision(position, size, velocity, onGround, deltaTime);
}
```

The Y-axis pass scans in the direction of travel so the entity always lands on the shallowest surface tile rather than tunneling through thin geometry.

```cpp
// World.cpp — falling branch
if (vel.y >= 0)
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
```

---

### Mining & Block Placement

Mining requires 3 hits spaced 0.25 seconds apart while holding left click. A progress bar and darkening overlay render over the targeted tile as it breaks. Mined tiles are added directly to the player's inventory map.

```cpp
// main.cpp — mining tick
mineTimer += deltaTime;
if (mineTimer >= mineInterval) {
    mineTimer -= mineInterval;
    mineProgress += 1.f;
    mineSound.play();
    if (mineProgress >= mineHitsRequired) {
        int tileType = world.getTile(mineRow, mineCol);
        world.setTile(mineRow, mineCol, 0);
        player.addToInventory(tileType, 1);
    }
}
```

Block placement (right click) validates two conditions before placing: the target cell must be empty, and it must be adjacent to an existing solid tile. It also checks that the tile does not overlap the player's bounding box, so you can't wall yourself in.

---

### Day/Night Cycle

The `DayNightCycle` class tracks a time value that loops over a configurable `dayLength` (120 seconds by default). The sun and moon travel along an ellipse centered on the camera; they are always on opposite sides.

```cpp
// DayNightCycle.cpp
float moveAngle = t * 2.f * 3.14159265f;
float sx = center.x + rx * std::cos(moveAngle - PI/2.f);
float sy = center.y + ry * std::sin(moveAngle - PI/2.f);
// Moon is exactly opposite the sun
float mx = center.x + rx * std::cos(moveAngle - PI/2.f + PI);
float my = center.y + ry * std::sin(moveAngle - PI/2.f + PI);
```

The sky color is a smooth blend between a daytime blue `(135, 206, 235)` and a night navy `(10, 12, 40)`, driven by `dayFactor`. A smoothstep ease (`u² * (3 - 2u)`) prevents sudden flips at sunrise and sunset.

---

### Enemy AI & Boss

Enemies patrol by walking in one direction and reversing when they hit a wall (`velocity.x == 0` after physics resolution). A boss spawns 60 seconds into each session via a countdown timer.

```cpp
// Enemies.cpp — patrol + wall bounce
void Enemy::update(float deltaTime, const World& world)
{
    if (!alive) return;
    velocity.x = instanceMoveSpeed * moveDir;
    applyPhysics(deltaTime, world);
    if (velocity.x == 0.f)   // hit a wall — reverse
        moveDir = -moveDir;
    sprite.setPosition(position);
}
```

The boss is the same `Enemy` class promoted via `setAsBoss()`, which triples its health, increases its size, raises its move speed, and switches its health bar to purple. Regular enemies respawn at a new random X position when killed; the boss does not.

---

### Combat — Sword & Projectile

Pressing Z triggers a melee swing and simultaneously fires a projectile aimed at the cursor. The projectile is a sprite flying at 750 px/s toward the mouse position; its angle is derived with `atan2` so it visually aligns with its travel direction.

```cpp
// Player.cpp — projectile launch
sf::Vector2f dir = aimPosition - hand;
float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
if (len > 0.f) dir /= len;

proj.velocity = dir * 750.f;
float angle = std::atan2(dir.y, dir.x) * (180.f / PI) + 45.f;
proj.sprite.setRotation(sf::degrees(angle));
```

Melee hit detection uses a `FloatRect` extending to the side the player is facing. Each frame, `main.cpp` tests every living enemy against that rect. Projectile hits are consumed (lifetime set to 0) on first contact so they don't deal damage to multiple enemies.

---

### Synthesized Audio

The mining click and sword swoosh sounds are generated at startup from raw PCM samples — no `.wav` files required for those effects. A decaying sine wave at a dropping frequency produces the mining thud; a sine-envelope frequency sweep produces the sword whoosh.

```cpp
// Player.cpp — sword swoosh generation
for (std::uint64_t i = 0; i < n; ++i) {
    float t        = static_cast<float>(i) / sampleRate;
    float progress = static_cast<float>(i) / static_cast<float>(n);
    float freq     = 600.f - 450.f * progress;          // sweeps 600→150 Hz
    float envelope = std::sin(progress * PI) * 0.7f;    // bell-shaped volume
    samples[i] = static_cast<std::int16_t>(
        std::sin(2.f * PI * freq * t) * envelope * 32767.f);
}
```

Background music uses `MusicPlayer` (a thin wrapper around `sf::Music`) with separate tracks for the menu and gameplay, swapping on state transitions.

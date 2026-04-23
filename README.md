# Final Project — Terraria-Inspired Game

[For group: Here is the link to the checklist - Remove link after final submission](https://trello.com/invite/b/69e9089c276e19c1b67adee2/ATTI5e3bdfb1b0367b4263326e64a3452410C828D335/finalproject)

A 2D sandbox survival game built in C++ with SFML 3, inspired by Terraria. Features include terrain mining, enemy AI, and a dynamic day/night cycle.

## Features (planned / in progress)

- Procedurally generated tile-based world
- Mining and block destruction
- Enemy spawning and AI
- Day/night cycle with dynamic lighting
- Player movement and collision

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

## Project Structure

```
Final Project/          — Visual Studio project & source files
include/SFML/           — SFML 3 headers
lib/                    — SFML 3 import and static libraries
x64/Debug/              — Debug build output (DLLs included)
x64/Release/            — Release build output (DLLs included)
```

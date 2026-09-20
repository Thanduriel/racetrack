Racetrack
===
An efficient but barebone implementation of the pen & paper game [Racetrack](https://en.wikipedia.org/wiki/Racetrack_(game)).
<img width="622" height="770" alt="Screenshot From 2026-09-20 13-16-15" src="https://github.com/user-attachments/assets/930c6336-8af0-4e62-a454-c841c07fb2eb" />

## Build
Requires a C++ 20 compiler. All dependencies are integrated to work right out of the box.
```bash
mkdir build && cd build
cmake .. -DWITH_GRAPHICS=ON
make -j
```
The dependencies are
* [lodepng](https://github.com/lvandeve/lodepng),
* [SFML 3](https://www.sfml-dev.org/) (only used if `WITH_GRAPHICS=ON`)
.

## How To
The choice of players and map is hard coded in `main.cpp`. If you want graphics, there has to be at least one `UIBot` in the race, since rendering only happens when waiting on user inputs.
If the map is to large to fit the screen, reduce the scaling factor of the `Renderer`.
When running the game make sure that the working directory is correct for the map path.

## Rules
There are slightly different versions of the game regarding:
 * *walls* A collision with a wall ends in a game over for the player.
 * *interactions* There are no collisions between players.

The plan is to add switches for alternative rules in the future.

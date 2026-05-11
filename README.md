# Snake Game

A classic Snake game built in C++ using OpenGL and GLUT, featuring a modern shader-based renderer, decorative wall layouts, a HUD, pause/game-over overlays, and a fixed-function fallback renderer for environments where shaders are unavailable.

![Snake Game Screenshot](screenshot.png)

---

## Features

- Smooth OpenGL rendering with GLSL shaders (vertex + fragment)
- Automatic fallback to fixed-function OpenGL if shaders fail to load
- HUD displaying current score and all-time best score
- Pause and game-over overlays
- Customizable wall layouts including a full border arena and decorative symmetric barriers
- Self-collision and wall-collision detection
- Clean separation of concerns across dedicated classes

---

## Project Structure

```
Snake/
├── src/
│   ├── main.cpp                  # Entry point, GLUT setup, input callbacks
│   ├── Common.hpp                # Shared constants, PairHash, GameState enum
│   ├── Game/
│   │   ├── Game.hpp              # Game state, scoring, update loop
│   │   └── Game.cpp
│   ├── Snake/
│   │   ├── Snake.hpp             # Snake body, movement, collision
│   │   └── Snake.cpp
│   ├── Fruit/
│   │   ├── Fruit.hpp             # Fruit placement and respawning
│   │   └── Fruit.cpp
│   ├── Wall/
│   │   ├── Wall.hpp              # Static wall set
│   │   └── Wall.cpp
│   └── Renderer/
│       ├── Renderer.hpp          # Renderer interface
│       └── Renderer.cpp          # OpenGL drawing implementation
├── shaders/
│   ├── vertex.glsl               # Scales and positions each grid cell
│   └── fragment.glsl             # Outputs solid colour per cell
├── obj/                          # Compiled object files (generated)
├── build/                        # Output executable (generated)
└── Makefile
```

---

## Dependencies

| Library | Purpose |
|---|---|
| OpenGL | Rendering |
| GLUT / FreeGLUT | Window creation, input, game loop timer |
| GLEW | Loading modern OpenGL function pointers (Windows/Linux only) |

### Installing dependencies on Windows (MSYS2)

```bash
pacman -S mingw-w64-x86_64-freeglut
pacman -S mingw-w64-x86_64-glew
```

### Installing dependencies on Linux

```bash
sudo apt install freeglut3-dev libglew-dev
```

### macOS

GLUT is included with Xcode. No extra installation needed. GLEW is not used on macOS.

---

## Building

```bash
make
```

This compiles all source files into `obj/` and links the executable to `build/SnakeGame.exe` (or `build/SnakeGame` on Linux/macOS).

### Running

```bash
make run
```

### Cleaning build artifacts

```bash
make clean
```

---

## Controls

| Key | Action |
|---|---|
| Arrow Keys | Steer the snake |
| P | Pause / Unpause |
| R | Restart after game over |
| ESC | Quit |

---

## Gameplay

- The snake grows by one segment each time it eats the red fruit
- Each fruit eaten scores 10 points
- The best score is preserved across restarts within the same session
- The snake dies if it hits a wall or itself
- The map is surrounded by a border wall — there is no wrapping

---

## Architecture Overview

| Class | Responsibility |
|---|---|
| `Game` | Owns Snake, Fruit, Wall. Drives the update loop, collision checks, scoring, and state transitions |
| `Snake` | Tracks body segments using a deque and a hash set. Handles movement, growth, and self-collision |
| `Fruit` | Scans all free grid cells and picks one at random for placement |
| `Wall` | Stores static wall positions in a hash set for O(1) collision lookup |
| `Renderer` | Reads game state and draws everything using OpenGL. Never modifies game state |
| `Common.hpp` | Single source of truth for grid size, window size, `PairHash`, and `GameState` |

---

## Shader Pipeline

The renderer uses a minimal two-stage GLSL pipeline:

- **vertex.glsl** — receives a unit square vertex, scales it to cell size, translates it to the correct pixel position, then converts to NDC
- **fragment.glsl** — outputs a solid colour set per draw call via the `uColor` uniform

If shader loading or compilation fails at startup, the renderer automatically falls back to the fixed-function OpenGL pipeline so the game remains playable.

---

## License

This project is open source. Feel free to use, modify, and distribute it.

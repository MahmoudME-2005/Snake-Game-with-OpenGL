# Snake Game

```
########################################################
#                                                      #
#      SCORE: 0042            HIGH SCORE: 0100         #
#                                                      #
#   ################################################   #
#   # . . . . . . . . . . . . . . . . . . . . . .  #   #
#   # . . . . . . . . . . . . . . . . . . . . . .  #   #
#   # . . . . . . . . █ █ █ █ █ █ . . . . . . . .  #   #
#   # . . . . . . . . █ . . . . . . . . . . . . .  #   #
#   # . . . ▣ . . . . █ . . . . . . . . . . . . .  #   #
#   # . . . . . . . . █ . . . . . . . . . . . . .  #   #
#   # . . . . █ █ █ █ █ . . . . . . . . . . . . .  #   #
#   # . . . . . . . . . . . . . . . . . . . . . .  #   #
#   # . . . . . . . . . . . . . . . . . . . . . .  #   #
#   ################################################   #
#                                                      #
#      CONTROLS: [W] UP  [S] DOWN  [A] LEFT  [D] RIGHT #
########################################################
```

---

## Group Members

| Name | ID |
|---|---|
| Mahmoud Ehab Moheb | 2301085 |
| Salsabil Ali Mahmoud | 2300127 |
| Mariam Maged Mohammad | 2300670 |
| Asmaa Salaheldin Abdelhamid Saleh | 2300181 |
| Youssef Sameh Nazir | 2300544 |

---

## Project Description

A classic Snake game implemented in C++ using OpenGL and GLUT. The player steers a growing snake around a walled arena, eating fruit to score points while avoiding walls and self-collision. The project was built as part of a Data Structures and Algorithms course, with a focus on selecting the right data structure for each game subsystem to achieve correct and efficient behavior.

The renderer uses a GLSL shader pipeline for drawing grid cells, with an automatic fallback to the fixed-function OpenGL pipeline if shaders fail to load. The game features a live HUD, pause and game-over overlays, a persistent best score, and a decorative symmetric wall layout inside a full border arena.

---

## Selected Data Structures and Why

### `std::deque<pair<int,int>>` — Snake Body

The snake's body is stored as a double-ended queue. Every move adds a new head to the front and removes the tail from the back. Both operations are O(1), which is exactly what a deque is optimized for. A vector would require shifting all elements on every move, making it O(n) per frame — unacceptable for a real-time game loop.

### `std::unordered_set<pair<int,int>, PairHash>` — Snake Body Set and Wall Set

Two hash sets run alongside the deque: one mirrors the snake body, one stores wall positions. Both exist purely for O(1) average-case collision lookup. Without them, checking whether the snake's next head position collides with its own body or a wall would require an O(n) linear scan through the deque or wall list every tick. A custom `PairHash` was implemented since the C++ standard library does not provide a default hash for `pair<int,int>`.

### `std::vector<pair<int,int>>` — Fruit Free-Cell Sampling

When placing the fruit, the algorithm scans every cell in the grid once (O(n)) and collects all cells not occupied by the snake or walls into a vector. It then picks a random index in O(1). This guarantees the fruit always lands on a valid free cell without repeated random retries that could loop indefinitely when the board is nearly full.

---

## Implemented Features

- GLSL shader-based OpenGL renderer with automatic fixed-function fallback
- Snake movement with queue-based head enqueue / tail dequeue logic
- Self-collision and wall-collision detection using O(1) hash set lookup
- Fruit placement using full free-cell scan to guarantee valid spawn positions
- Full border arena wall with decorative symmetric interior barriers
- Live HUD showing current score and best score
- Pause overlay (P key) and game-over overlay with restart support (R key)
- Best score preserved across restarts within the same session
- Configurable snake speed via `TICK_MS` constant in `main.cpp`

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

---

## How to Compile and Run

### Windows (MSYS2 / MinGW)

**1. Install dependencies:**
```bash
pacman -S mingw-w64-x86_64-freeglut
pacman -S mingw-w64-x86_64-glew
```

**2. Build:**
```bash
make
```

**3. Run:**
```bash
make run
```

**4. Clean build artifacts:**
```bash
make clean
```

### Linux

**1. Install dependencies:**
```bash
sudo apt install freeglut3-dev libglew-dev
```

**2. Build and run:**
```bash
make
make run
```

### macOS

GLUT is bundled with Xcode. GLEW is not needed on macOS.

```bash
make
make run
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

## AI Usage Declaration

This project was developed with the assistance of Claude (Anthropic) as an AI pair-programming tool. AI was used to:

- Review and debug code across all project files
- Generate and refine the `Game` class, `main.cpp`, and `Common.hpp`
- Identify architectural issues such as duplicate `PairHash` definitions and mismatched getter names between classes
- Fix Makefile linking errors
- Identify and fix the `uWinSize` uniform being looked up every frame instead of being cached
- Generate the wall layout and this README

All AI-suggested code was reviewed, understood, and integrated by the team. The data structure choices, game logic, and overall architecture were designed and directed by the group.

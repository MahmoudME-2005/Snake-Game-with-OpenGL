#pragma once

/*
===============================================================================
File: Common.hpp
Role: Shared constants, types, and utilities used across the whole project.

Put here:
 - Window/grid constants so every file reads from one place
 - PairHash so Snake, Wall, and Fruit all use the same struct
 - GameState enum so Game and Renderer agree on state values
===============================================================================
*/

// ── Window & grid constants ──────────────────────────────────────────────────
static constexpr int CELL   = 20;       // pixel size of one grid cell
static constexpr int GRID_W = 30;       // grid columns
static constexpr int GRID_H = 30;       // grid rows
static constexpr int HUD_H  = 40;       // pixel height of the top HUD bar
static constexpr int WIN_W  = GRID_W * CELL;          // 600
static constexpr int WIN_H  = GRID_H * CELL + HUD_H;  // 640

// ── Shared hash for pair<int,int> ────────────────────────────────────────────
// Previously defined separately in Snake.hpp (PairHash) and Wall.hpp (pairHash).
// Having two structs caused type mismatches when sets were passed between files.
// Define it once here and include Common.hpp everywhere instead.
#include <utility>
#include <functional>

struct PairHash
{
    std::size_t operator()(const std::pair<int,int>& p) const
    {
        // Shift one value to reduce collisions on small grids
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 16);
    }
};

// ── Game state ───────────────────────────────────────────────────────────────
// Shared between Game.hpp and Renderer.cpp.
// Renderer checks this to decide whether to draw the overlay.
enum class GameState
{
    PLAYING,
    PAUSED,
    GAME_OVER
};

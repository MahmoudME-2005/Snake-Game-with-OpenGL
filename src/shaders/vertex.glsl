#version 330 core

/*
===============================================================================
 File: vertex.glsl
 Role: Vertex shader used by Renderer.cpp for drawing one grid cell at a time.

 What this file does:
   - Receives a unit-square vertex position from Renderer.cpp.
   - Scales the unit square to the current cell size.
   - Moves the cell to the required pixel position.
   - Converts pixel coordinates into OpenGL normalized device coordinates (NDC).

 What this file expects from Renderer.cpp:
   - Attribute location 0 must contain vec2 vertex positions for a unit square:
       (0,0), (1,0), (1,1), (0,1)
   - Uniforms must be set before drawing:
       uOffset   = bottom-left pixel position of the cell
       uCellSize = width/height of the cell in pixels
       uWinSize  = full window size in pixels

 What other files use from it:
   - Renderer.cpp loads, compiles, and links this shader during Renderer::init().
===============================================================================
*/

// Unit-square vertex (0..1, 0..1)
layout (location = 0) in vec2 aPos;

// Per-draw uniforms set by Renderer::drawCellShader()
uniform vec2 uOffset;    // pixel offset (bottom-left of cell after inset)
uniform vec2 uCellSize;  // pixel size of the cell after inset
uniform vec2 uWinSize;   // full window size in pixels (WIN_W, WIN_H)

void main() {
    // Scale unit square to cell size, then translate to offset
    vec2 pixelPos = aPos * uCellSize + uOffset;

    // Convert pixel coords → NDC  (y is already OpenGL-bottom-up)
    vec2 ndc = (pixelPos / uWinSize) * 2.0 - 1.0;

    gl_Position = vec4(ndc, 0.0, 1.0);
}

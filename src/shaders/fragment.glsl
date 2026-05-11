#version 330 core

/*
===============================================================================
 File: fragment.glsl
 Role: Fragment shader used by Renderer.cpp for colouring grid cells

 What this file does:
   - Outputs one solid colour for the current drawn cell

 What this file expects from Renderer.cpp:
   - Renderer.cpp must set uColor before each draw call

 What other files use from it:
   - Renderer.cpp loads, compiles, and links this shader during Renderer::init()
===============================================================================
*/

out vec4 FragColor;

// RGB colour provided by Renderer.cpp for the current object/cell
uniform vec3 uColor;  

void main() {
    FragColor = vec4(uColor, 1.0);
}
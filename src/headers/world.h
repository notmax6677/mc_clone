#ifndef WORLD_H
#define WORLD_H

#include <GLAD33/glad.h>
#include <GLFW/glfw3.h>

#include "chunk.h"

struct Chunk get_chunk(int xPos, int yPos);

// initiate world
void init_world();

// update world
void update_world(GLFWwindow* window);

// draw world
void draw_world();

// getters

// gets the current wireframe mode
bool get_wireframe_mode();

// setters

// sets the wireframe mode
void set_wireframe_mode(bool setting);

#endif

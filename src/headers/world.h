#ifndef WORLD_H
#define WORLD_H

#include <GLAD33/glad.h>
#include <GLFW/glfw3.h>

#include "chunk.h"

// gets the chunk object based on the snapped chunks position
struct Chunk get_chunk(int xPos, int yPos);

// gets the index of a chunk based on the snapped chunks position
int get_chunk_index(int xPos, int yPos);

// sets an indexed chunk to an inserted chunk object
void set_chunk(int index, struct Chunk* chunk);

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

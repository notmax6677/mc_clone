#ifndef POINTER_H
#define POINTER_H

#include <GLAD33/glad.h>
#include <GLFW/glfw3.h>

// initiates pointer related stuff
void init_pointer();

// updates pointer related stuff
void update_pointer(GLFWwindow* window);

// draws pointer related stuff
void draw_pointer(unsigned int worldAtlas);

// pointer mouse input callback
void pointer_mouse_input_callback(GLFWwindow* window, int button, int action, int mods);

#endif

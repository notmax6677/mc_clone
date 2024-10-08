#ifndef MENU_H
#define MENU_H

#include "../include/GLAD33/glad.h"
#include "../include/GLFW/glfw3.h"
#include "../include/CGLM/cglm.h"

// getter for selectedItem
int get_selected_item();

// initiates menu related stuff
void init_menu();

// updates menu
void update_menu(GLFWwindow* window);

// draws the menu
void draw_menu(GLFWwindow* window, unsigned int worldAtlas);

#endif

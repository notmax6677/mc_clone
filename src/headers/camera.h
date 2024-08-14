#ifndef CAMERA_H
#define CAMERA_H

#include "../include/GLAD33/glad.h"
#include "../include/GLFW/glfw3.h"
#include <CGLM/cglm.h>

// centres the camera by positioning it in the centre of the world
void centre_cam_pos(int world_size, int chunk_width, int chunk_length);

// toggles the zoom of the camera
void toggle_zoom();

// callback for when the window is resized, but regarding specifically camera related processes
void camera_resize_callback(int width, int height);

// mouse position callback but only for camera processes
void camera_mouse_callback(GLFWwindow* window, double xpos, double ypos);

// initiates camera related stuff
void init_camera(GLFWwindow* window);

// update camera processes and positioning
void update_camera(GLFWwindow* window, float deltaTime);

// getters
mat4* get_model();
mat4* get_view();
mat4* get_projection();

vec3* get_camera_pos();
vec3* get_camera_front();

bool get_cursor_lock();

// setters
void set_model(mat4* matrix); 
void set_view(mat4* matrix);
void set_projection(mat4* matrix);

void set_cursor_lock(GLFWwindow* window, bool lock);

#endif

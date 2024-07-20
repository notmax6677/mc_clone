#ifndef CAMERA_H
#define CAMERA_H

#include <GLAD33/glad.h>
#include <GLFW/glfw3.h>
#include <CGLM/cglm.h>

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

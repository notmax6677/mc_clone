#ifndef IMAGE_H
#define IMAGE_H

// loads an image texture and returns the gl reference object to it
unsigned int load_texture(char* path);

// calculate atlas texture x coordinate
float calc_at_tex_x(float x);

// calculate atlas texture x coordinate
float calc_at_tex_y(float y);

#endif

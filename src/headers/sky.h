#ifndef SKY_H
#define SKY_H

#include <CGLM/cglm.h>

// returns the current block shading
float get_block_shading();

// getter for sky color
vec3* get_sky_col();

// initiates sky related processes
void init_sky();

// updates sky related stuff
void update_sky(float deltaTime);

// draws the sky
void draw_sky(unsigned int worldAtlas);

#endif

#ifndef SKY_H
#define SKY_H

#include <CGLM/cglm.h>

// returns the current block shading
float get_block_shading();

// getter for sky color
vec3* get_sky_col();

// getter for whether or not the current day/night cycle is active
bool get_day_night_cycle();

// getter for tide level
float get_tide_level();

// setter for active day/night cycle
void set_day_night_cycle(bool value);

// initiates sky related processes
void init_sky();

// updates sky related stuff
void update_sky(float deltaTime);

// draws the sky
void draw_sky(unsigned int worldAtlas, bool drawingWater);

#endif

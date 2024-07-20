#ifndef CHUNK_H
#define CHUNK_H

#include <CGLM/cglm.h>

#include "mesh.h"

// whether or not the player is under the water level
bool get_underwater_level();

// chunk structure
struct Chunk {
	vec2 pos; // multiplied by 16
	
	int sides; // how many sides do render (used when drawing it)
	
	int* blockTypes; // array of block types
	
	struct Mesh mesh; // mesh of chunk
};

// generates a chunk
struct Chunk generate_chunk(vec2 position, bool water);

// draw a chunk
void draw_chunk(struct Chunk chunk, unsigned int shaderProgram, unsigned int worldAtlas);

#endif

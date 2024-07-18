#ifndef CHUNK_H
#define CHUNK_H

#include <CGLM/cglm.h>

#include "mesh.h"

// chunk structure
struct Chunk {
	vec2 pos; // multiplied by 16
	
	int blockCount; // amount of blocks
	
	// block positions
	vec4* blockPositions;
	
	struct Mesh mesh; // mesh of chunk
};

// generates a chunk
struct Chunk generate_chunk();

// draw a chunk
void draw_chunk(struct Chunk chunk, unsigned int shaderProgram, unsigned int worldAtlas);

#endif

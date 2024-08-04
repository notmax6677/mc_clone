#ifndef CHUNK_H
#define CHUNK_H

#include <CGLM/cglm.h>

#include "mesh.h"

// randomizes the noise offset
void randomizeNoiseOffset();

// getters for chunk sizes
int get_chunk_width();
int get_chunk_length();
int get_chunk_height();

// just returns the constant value of the water level float
float get_water_level();

// whether or not the player is under the water level - getter and setter
bool get_under_water_level();
void set_under_water_level(bool value);

// calculates noise value as integer block y coordinate at given position, allows for offsetting with chunk coords
int calc_chunk_noise_value(vec2 position, vec2* chunkOffset);

// gets the block type of a block within a chunk
int get_block_type(int* blockTypes, int xPos, int yPos, int zPos);

// chunk structure
struct Chunk {
	vec2 pos; // multiplied by CHUNK_WIDTH and CHUNK_HEIGHT
	
	int sides; // how many sides to render (used when drawing it)
	
	int* blockTypes; // array of block types
	
	struct Mesh mesh; // mesh of chunk
};

// inserts a block into a chunk
void insert_block(struct Chunk* chunk, struct Chunk* leftChunk, struct Chunk* rightChunk, struct Chunk* topChunk, struct Chunk* bottomChunk, vec4 block);

// generates a chunk
struct Chunk generate_chunk(vec2 position, bool water);

// optimize the chunks to remove internal faces
void handle_chunk_sides(struct Chunk* chunk, struct Chunk* leftChunk, struct Chunk* rightChunk, struct Chunk* topChunk, struct Chunk* bottomChunk);

// draw a chunk
void draw_chunk(struct Chunk chunk, unsigned int shaderProgram, unsigned int worldAtlas, bool water);

#endif

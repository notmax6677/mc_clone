#include <CGLM/vec2.h>
#include <GLAD33/glad.h>
#include <GLFW/glfw3.h>
#include <CGLM/cglm.h>

#include <stdio.h>
#include <string.h>

#include "headers/image.h"
#include "headers/camera.h"
#include "headers/shader.h"

#include "headers/test_block.h"
#include "headers/chunk.h"


// ---


// draw chunks in wireframe mode
bool wireframeMode = false;

// render distance of chunks
const int RENDER_DISTANCE = 9;

// keep track of last chunk position of player
vec2 lastChunkPos = GLM_VEC2_ZERO;


// ---


// world texture atlas
unsigned int worldAtlas;
// shader program for blocks
unsigned int blockShaderProgram;


// ---


struct Chunk firstChunk;

struct Chunk* chunks;

int chunkCount = 0;


// ---


void init_world() {
	// create shaderprogram
	blockShaderProgram = create_shader_program("shaders/block_shader.vert", "shaders/block_shader.frag");

	// create texture atlas object
	worldAtlas = load_texture("assets/atlas.png");

	// allocate size to chunks
	chunks = calloc(RENDER_DISTANCE*RENDER_DISTANCE, sizeof(struct Chunk));

	// coordinates for drawing chunks (from a 2D top-down perspective)
	int xPos = 0;
	int yPos = 0;

	// iterate thru x and z based on render distance
	for(int i = 0; i < RENDER_DISTANCE*RENDER_DISTANCE; i++) {
		// generate indexed chunk
		chunks[chunkCount] = generate_chunk((vec2){xPos - floor(RENDER_DISTANCE/2), yPos - floor(RENDER_DISTANCE/2)});

		chunkCount++;


		// ---


		// handle incrementing xPos and yPos
		xPos++;
		if(xPos >= RENDER_DISTANCE) {
			xPos = 0;
			yPos++;
		}
	}

	init_test_block();

	//firstChunk = generate_chunk((vec2){1, 1});
}

void update_world() {
	// fetch camera position
	vec3* camPos = get_camera_pos();

	// snap player position to chunk grid and assign data to vec2 object
	vec2 playerChunkPos;
	glm_vec2_copy(
			(vec2) { floor((*camPos)[0] / 16), floor((*camPos)[2] / 16)}, 
			playerChunkPos);
	

	if(playerChunkPos[0] == lastChunkPos[0]+1) {

	
		//chunks[chunkCount] = generate_chunk(
		//		(vec2){ playerChunkPos[0], playerChunkPos[1] });


		//chunkCount++;

	}


	// at end of function, copy over player chunk position to lastChunkPos for next frame
	glm_vec2_copy(playerChunkPos, lastChunkPos);
}

void draw_world() {
	draw_chunk(firstChunk, blockShaderProgram, worldAtlas);

	//draw_test_block(blockShaderProgram, worldAtlas);
	
	// iterate thru x and z based on render distance
	for(int i = 0; i < chunkCount; i++) {
	
		draw_chunk(chunks[i], blockShaderProgram, worldAtlas);

	}
}


// ---


// getters

// gets the current wireframe mode
bool get_wireframe_mode() {
 	return wireframeMode;
}

// setters

// sets the wireframe mode
void set_wireframe_mode(bool setting) {
	wireframeMode = setting;
}

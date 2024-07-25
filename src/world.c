#include <CGLM/vec4.h>
#include <GLAD33/glad.h>
#include <GLFW/glfw3.h>
#include <CGLM/cglm.h>

#include <NOISE/noise1234.h>

#include <stdio.h>
#include <string.h>

#include "headers/image.h"
#include "headers/camera.h"
#include "headers/shader.h"

#include "headers/test_block.h"

#include "headers/pointer.h"
#include "headers/chunk.h"


// ---


// draw chunks in wireframe mode
bool wireframeMode = false;

// render distance of chunks
const int RENDER_DISTANCE = 6;
const int WORLD_SIZE = 20;

// keep track of last chunk position of player
vec2 lastChunkPos = GLM_VEC2_ZERO;


// ---


// world texture atlas
unsigned int worldAtlas;
// shader program for blocks
unsigned int blockShaderProgram;


// ---


struct Chunk* chunks;

struct Chunk* waterChunks;

int chunkCount = 0;


// ---


// gets a chunk based on the snapped chunks position
struct Chunk get_chunk(int xPos, int yPos) {
	for(int i=0; i < WORLD_SIZE*WORLD_SIZE-1; i++) {
		if(chunks[i].pos[0] == xPos && chunks[i].pos[1] == yPos) {
			return chunks[i];
		}
	}
	return chunks[0]; // by default return first chunk
}


// ---


void init_world() {
	// create shaderprogram
	blockShaderProgram = create_shader_program("shaders/block_shader.vert", "shaders/block_shader.frag");

	// create texture atlas object
	worldAtlas = load_texture("assets/atlas.png");

	// initiate pointer related stuff
	init_pointer();

	// randomize noise offset for chunk generation
	randomizeNoiseOffset();

	// allocate size to chunks
	chunks = calloc(WORLD_SIZE*WORLD_SIZE, sizeof(struct Chunk));
	// and watercChunks
	waterChunks = calloc(WORLD_SIZE*WORLD_SIZE, sizeof(struct Chunk));

	// coordinates for drawing chunks (from a 2D top-down perspective)
	int xPos = 0;
	int yPos = 0;

	// iterate thru x and z based on render distance
	for(int i = 0; i < WORLD_SIZE*WORLD_SIZE; i++) {
		// generate indexed chunk
		chunks[chunkCount] = generate_chunk((vec2){xPos - floor(WORLD_SIZE/2), yPos - floor(WORLD_SIZE/2)}, false, GLM_VEC4_ZERO);

		// generate indexed chunk, but now for water part of the chunk
		waterChunks[chunkCount] = generate_chunk((vec2){xPos - floor(WORLD_SIZE/2), yPos - floor(WORLD_SIZE/2)}, true, GLM_VEC4_ZERO);

		chunkCount++;


		// ---


		// handle incrementing xPos and yPos
		xPos++;
		if(xPos >= WORLD_SIZE) {
			xPos = 0;
			yPos++;
		}
	}

	init_test_block();
}

void update_world(GLFWwindow* window) {
	// fetch camera position
	vec3* camPos = get_camera_pos();

	// snap player position to chunk grid and assign data to vec2 object
	vec2 playerChunkPos;
	glm_vec2_copy(
			(vec2) { floor((*camPos)[0] / 16), floor((*camPos)[2] / 16)}, 
			playerChunkPos);


	// at end of function, copy over player chunk position to lastChunkPos for next frame
	glm_vec2_copy(playerChunkPos, lastChunkPos);

	update_pointer(window);
}

void draw_world() {

	//draw_test_block(blockShaderProgram, worldAtlas);
	
	// get camera position
	vec3* camPos = get_camera_pos();

	// calculate noise value
	int noiseValue = calc_chunk_noise_value( (vec2){(*camPos)[0], (*camPos)[2]}, GLM_VEC2_ZERO );

	// check if camera is under water level and adjust underWaterLevel boolean accordingly
	if((*camPos)[1] <= get_water_level() && (*camPos)[1] > noiseValue) {
		set_under_water_level(true);
	}
	else {
		set_under_water_level(false);
	}


	// ---
		

	// iterate thru x and z based on render distance
	for(int i = 0; i < chunkCount; i++) {
		
		if((chunks[i].pos[0] < lastChunkPos[0]+RENDER_DISTANCE
			&& chunks[i].pos[0] > lastChunkPos[0]-RENDER_DISTANCE)
			&& (chunks[i].pos[1] < lastChunkPos[1]+RENDER_DISTANCE
			&& chunks[i].pos[1] > lastChunkPos[1]-RENDER_DISTANCE)) {

			draw_chunk(chunks[i], blockShaderProgram, worldAtlas);


		}
	}


	// if not underwater
	if(!get_under_water_level()) {
		// iterate thru x and z based on render distance
		for(int i = 0; i < chunkCount; i++) {
			
			if((chunks[i].pos[0] < lastChunkPos[0]+RENDER_DISTANCE
				&& chunks[i].pos[0] > lastChunkPos[0]-RENDER_DISTANCE)
				&& (chunks[i].pos[1] < lastChunkPos[1]+RENDER_DISTANCE
				&& chunks[i].pos[1] > lastChunkPos[1]-RENDER_DISTANCE)) {


				draw_chunk(waterChunks[i], blockShaderProgram, worldAtlas);

			}
		}
	}


	// ---
	

	draw_pointer(worldAtlas);



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

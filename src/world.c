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
#include "headers/sky.h"


// ---


// draw chunks in wireframe mode
bool wireframeMode = false;

// render distance of chunks
const int RENDER_DISTANCE = 3;
const int WORLD_SIZE = 10;

// keep track of last chunk position of player
vec2 lastChunkPos = GLM_VEC2_ZERO;


// ---


// world texture atlas
unsigned int worldAtlas;
// shader program for blocks
unsigned int blockShaderProgram;


// ---


// array of chunks with land blocks
struct Chunk* chunks;

// array of chunks with water blocks
struct Chunk waterChunk;

int chunkCount = 0;

// array that holds the indexes of chunks in the order that they should be drawn
int* chunksDrawOrder;


// ---


// whether or not to render the water
bool drawingWater = true;


// ---


// toggles drawing water mode
void toggle_drawing_water() {
	drawingWater = !drawingWater;
}


// ---


// getter for world size
int get_world_size() {
	return WORLD_SIZE;
}


// ---


// getter for the value of the world atlas
unsigned int get_world_atlas() {
	return worldAtlas;
}


// ---


// sorts a chunks array based on distance from camera, back to front
void sortChunks() {
	// create new vec2 array, x for chunk index, y for distance from player
	vec2 chunksData[WORLD_SIZE*WORLD_SIZE];


	// ---
	

	// get camera position
	vec3* camPos = get_camera_pos();

	// create vector2 and load camera position but snapped to chunk position
	vec2 snappedCamPos;
	glm_vec2_copy((vec2){
		round( (*camPos)[0] / get_chunk_width() ),
		round( (*camPos)[2] / get_chunk_length() )
	}, snappedCamPos);

	// iterate and store array vector2s of index and distance from player into chunksData
	for(int i=0; i < WORLD_SIZE*WORLD_SIZE; i++) {
	
		// get distance of chunk from camera (pythagoras theorem)
		float dist = sqrt( 
				snappedCamPos[0] - chunks[i].pos[0]
				+
				snappedCamPos[1] - chunks[i].pos[1]
		);

		// load index and distance into correspondingly indexed vector
		glm_vec2_copy((vec2){i, dist}, chunksData[i]);

	}

	// now actually sort the vectors from lowest to highest distance
	
	// boolean to recognize when bubble sort has fully completed
	bool sorted = false;

	// amount of swaps per iteration
	int swaps = 0;

	// while unsorted
	while(!sorted) {

		// reset swaps to 0
		swaps = 0;

		// iterate thru chunksData array
		for(int i=0; i < WORLD_SIZE*WORLD_SIZE; i++) {
		
			// if not the last element
			if(i != WORLD_SIZE*WORLD_SIZE-1) {
				
				// get and store copies of first and second vectors
				vec2 firstVec;
				glm_vec2_copy(chunksData[i], firstVec);
				vec2 secondVec;
				glm_vec2_copy(chunksData[i+1], secondVec); // next one
				
				// if chunk in front has lower distance value
				if(firstVec[1] > secondVec[1]) {

					// swap the vectors in position within the chunksData array
					glm_vec2_copy(firstVec, chunksData[i+1]);
					glm_vec2_copy(secondVec, chunksData[i]);

					// increment swaps
					swaps++;

				}

			}

		}

		// if no swaps were made then array is sorted
		if(swaps == 0) {
			sorted = true;
		}

	}

	// now fill newChunks array with chunks from inputChunks, but in backwards order based on chunksData
	
	// iterate thru chunksData
	for(int i=0; i < WORLD_SIZE*WORLD_SIZE; i++) {

		// set index of chunksDrawOrder integer
		chunksDrawOrder[i] = chunksData[i][0];

	}

}


// ---


// gets a chunk based on the snapped chunks position
struct Chunk* get_chunk(int xPos, int yPos) {
	for(int i=0; i < WORLD_SIZE*WORLD_SIZE; i++) {
		if(chunks[i].pos[0] == xPos && chunks[i].pos[1] == yPos) {
			return &(chunks[i]);
		}
	}
	return NULL; // by default return NULL
}

// gets the index of a chunk based on the snapped chunks position
int get_chunk_index(int xPos, int yPos) {
	for(int i=0; i < WORLD_SIZE*WORLD_SIZE; i++) {
		if(chunks[i].pos[0] == xPos && chunks[i].pos[1] == yPos) {
			return i;
		}
	}
	return 0; // by default return first chunk
}

// sets an indexed chunk to an inserted chunk object
void set_chunk(int index, struct Chunk* chunk) {
	chunks[index] = *chunk;
}


// ---


void init_world() {
	// create shaderprogram
	blockShaderProgram = create_shader_program("shaders/block_shader.vert", "shaders/block_shader.frag");

	// create texture atlas object
	worldAtlas = load_texture("assets/atlas.png");

	// initiate pointer related stuff
	init_pointer();

	// initiate sky related stuff
	init_sky();

	// randomize noise offset for chunk generation
	randomize_noise_offset();

	// allocate size to chunks
	chunks = calloc(WORLD_SIZE*WORLD_SIZE, sizeof(struct Chunk));

	// allocate for chunksDrawOrder
	chunksDrawOrder = calloc(WORLD_SIZE*WORLD_SIZE, sizeof(int));

	// coordinates for drawing chunks (from a 2D top-down perspective)
	int xPos = 0;
	int yPos = 0;

	// generate water chunk
	waterChunk  = generate_chunk((vec2){0, 0}, WORLD_SIZE, true);

	// iterate thru x and z based on render distance
	for(int i = 0; i < WORLD_SIZE*WORLD_SIZE; i++) {
		// generate indexed chunk
		chunks[chunkCount] = generate_chunk((vec2){xPos, yPos}, WORLD_SIZE, false);

		// increment chunk count
		chunkCount++;


		// ---


		// handle incrementing xPos and yPos
		xPos++;
		if(xPos >= WORLD_SIZE) {
			xPos = 0;
			yPos++;
		}
	}

	// iterate thru the new chunks again
	for(int i = 0; i < WORLD_SIZE*WORLD_SIZE; i++) {

		// declare necessary chunks for side handling
		struct Chunk chunk;
		struct Chunk* leftChunk;
		struct Chunk* rightChunk;
		struct Chunk* topChunk;
		struct Chunk* bottomChunk;

		// if not on the edges
		if( chunks[i].pos[0] != 0 && chunks[i].pos[0] != WORLD_SIZE-1
			&& chunks[i].pos[1] != 0 && chunks[i].pos[1] != WORLD_SIZE-1) {

			// get and define main chunk and surrounding chunks
			chunk       = chunks[i];
			leftChunk   = get_chunk(chunks[i].pos[0]-1, chunks[i].pos[1]);
			rightChunk  = get_chunk(chunks[i].pos[0]+1, chunks[i].pos[1]);
			topChunk    = get_chunk(chunks[i].pos[0],   chunks[i].pos[1]+1);
			bottomChunk = get_chunk(chunks[i].pos[0],   chunks[i].pos[1]-1);
		
			// handle chunk sides for the indexed chunk
			handle_chunk_sides(&chunk, leftChunk, rightChunk, topChunk, bottomChunk);	

		}
		// bottom left corner
		else if(chunks[i].pos[0] == 0 && chunks[i].pos[1] == 0) {

			// get and define main chunk and surrounding chunks
			chunk       = chunks[i];
			rightChunk  = get_chunk(chunks[i].pos[0]+1, chunks[i].pos[1]);
			topChunk    = get_chunk(chunks[i].pos[0],   chunks[i].pos[1]+1);
		
			// handle chunk sides for the indexed chunk (pass NULL for nonexistent surrounding chunks)
			handle_chunk_sides(&chunk, NULL, rightChunk, topChunk, NULL);	

		}
		// bottom right corner
		else if(chunks[i].pos[0] == WORLD_SIZE-1 && chunks[i].pos[1] == 0) {

			// get and define main chunk and surrounding chunks
			chunk       = chunks[i];
			leftChunk   = get_chunk(chunks[i].pos[0]-1, chunks[i].pos[1]);
			topChunk    = get_chunk(chunks[i].pos[0],   chunks[i].pos[1]+1);
		
			// handle chunk sides for the indexed chunk (pass NULL for nonexistent surrounding chunks)
			handle_chunk_sides(&chunk, leftChunk, NULL, topChunk, NULL);	

		}
		// top left corner
		else if(chunks[i].pos[0] == 0 && chunks[i].pos[1] == WORLD_SIZE-1) {

			// get and define main chunk and surrounding chunks
			chunk       = chunks[i];
			rightChunk  = get_chunk(chunks[i].pos[0]+1, chunks[i].pos[1]);
			bottomChunk = get_chunk(chunks[i].pos[0],   chunks[i].pos[1]-1);
		
			// handle chunk sides for the indexed chunk (pass NULL for nonexistent surrounding chunks)
			handle_chunk_sides(&chunk, NULL, rightChunk, NULL, bottomChunk);	

		}
		// top right corner
		else if(chunks[i].pos[0] == WORLD_SIZE-1 && chunks[i].pos[1] == WORLD_SIZE-1) {

			// get and define main chunk and surrounding chunks
			chunk       = chunks[i];
			leftChunk   = get_chunk(chunks[i].pos[0]-1, chunks[i].pos[1]);
			bottomChunk = get_chunk(chunks[i].pos[0],   chunks[i].pos[1]-1);
		
			// handle chunk sides for the indexed chunk (pass NULL for nonexistent surrounding chunks)
			handle_chunk_sides(&chunk, leftChunk, NULL, NULL, bottomChunk);	

		}
		// left side
		else if(chunks[i].pos[0] == 0) {

			// get and define main chunk and surrounding chunks
			chunk       = chunks[i];
			rightChunk  = get_chunk(chunks[i].pos[0]+1, chunks[i].pos[1]);
			topChunk    = get_chunk(chunks[i].pos[0],   chunks[i].pos[1]+1);
			bottomChunk = get_chunk(chunks[i].pos[0],   chunks[i].pos[1]-1);
		
			// handle chunk sides for the indexed chunk (pass corresponding nonexistent side chunk as NULL)
			handle_chunk_sides(&chunk, NULL, rightChunk, topChunk, bottomChunk);	

		}
		// right side
		else if(chunks[i].pos[0] == WORLD_SIZE-1) {

			// get and define main chunk and surrounding chunks
			chunk       = chunks[i];
			leftChunk   = get_chunk(chunks[i].pos[0]-1, chunks[i].pos[1]);
			topChunk    = get_chunk(chunks[i].pos[0],   chunks[i].pos[1]+1);
			bottomChunk = get_chunk(chunks[i].pos[0],   chunks[i].pos[1]-1);
		
			// handle chunk sides for the indexed chunk (pass corresponding nonexistent side chunk as NULL)
			handle_chunk_sides(&chunk, leftChunk, NULL, topChunk, bottomChunk);	

		}
		// top side
		else if(chunks[i].pos[1] == WORLD_SIZE-1) {

			// get and define main chunk and surrounding chunks
			chunk       = chunks[i];
			leftChunk   = get_chunk(chunks[i].pos[0]-1, chunks[i].pos[1]);
			rightChunk  = get_chunk(chunks[i].pos[0]+1, chunks[i].pos[1]);
			bottomChunk = get_chunk(chunks[i].pos[0],   chunks[i].pos[1]-1);
		
			// handle chunk sides for the indexed chunk (pass corresponding nonexistent side chunk as NULL)
			handle_chunk_sides(&chunk, leftChunk, rightChunk, NULL, bottomChunk);	

		}
		// bottom side
		else if(chunks[i].pos[1] == 0) {

			// get and define main chunk and surrounding chunks
			chunk       = chunks[i];
			leftChunk   = get_chunk(chunks[i].pos[0]-1, chunks[i].pos[1]);
			rightChunk  = get_chunk(chunks[i].pos[0]+1, chunks[i].pos[1]);
			topChunk    = get_chunk(chunks[i].pos[0],   chunks[i].pos[1]+1);
		
			// handle chunk sides for the indexed chunk (pass corresponding nonexistent side chunk as NULL)
			handle_chunk_sides(&chunk, leftChunk, rightChunk, topChunk, NULL);	

		}

	}

	// initiate test block related stuff
	init_test_block();

	
	// ---
	

	// sort chunk draw order
	sortChunks();


	// ---
	

	// centre the camera in the middle of world
	centre_cam_pos(WORLD_SIZE, get_chunk_width(), get_chunk_length());
}

void update_world(GLFWwindow* window, float deltaTime) {
	// fetch camera position
	vec3* camPos = get_camera_pos();

	// snap player position to chunk grid and assign data to vec2 object
	vec2 playerChunkPos;
	glm_vec2_copy(
			(vec2) { 
				round((*camPos)[0] / get_chunk_width()), 
				round((*camPos)[2] / get_chunk_length())
			}, 
			playerChunkPos);

	// snap chunk positions to world edges
	if(playerChunkPos[0] <= 0) {
		playerChunkPos[0] = 0;
	}
	if(playerChunkPos[0] >= WORLD_SIZE-1) {
		playerChunkPos[0] = WORLD_SIZE-1;
	}
	if(playerChunkPos[1] <= 0) {
		playerChunkPos[1] = 0;
	}
	if(playerChunkPos[1] >= WORLD_SIZE-1) {
		playerChunkPos[1] = WORLD_SIZE-1;
	}

	// if moved to another chunk
	if(playerChunkPos[0] != lastChunkPos[0] || playerChunkPos[1] != lastChunkPos[1]) {
		sortChunks();
	}


	// at end of function, copy over player chunk position to lastChunkPos for next frame
	glm_vec2_copy(playerChunkPos, lastChunkPos);

	update_sky(deltaTime);

	update_pointer(window);
}

void draw_world(GLFWwindow* window) {
	
	// draw the sky
	draw_sky(worldAtlas, drawingWater);
	

	// ---

	
	// get camera position
	vec3* camPos = get_camera_pos();

	// calculate noise value
	int noiseValue = calc_chunk_noise_value( (vec2){(*camPos)[0], (*camPos)[2]}, GLM_VEC2_ZERO );

	// check if camera is under water level and adjust underWaterLevel boolean accordingly
	if((*camPos)[1] <= get_water_level() + get_tide_level()) {
		set_under_water_level(true);
	}
	else {
		set_under_water_level(false);
	}


	// ---
		

	// iterate thru x and z based on render distance
	for(int i = 0; i < chunkCount; i++) {

		int index = (int)chunksDrawOrder[i];
		
		if((chunks[index].pos[0] < lastChunkPos[0]+RENDER_DISTANCE
			&& chunks[index].pos[0] >= lastChunkPos[0]-RENDER_DISTANCE)
			&& (chunks[index].pos[1] < lastChunkPos[1]+RENDER_DISTANCE
			&& chunks[index].pos[1] >= lastChunkPos[1]-RENDER_DISTANCE)) {

			draw_chunk(chunks[index], blockShaderProgram, worldAtlas, false, drawingWater);


		}
	}


	// ---


	// draw select block
	draw_select_block(worldAtlas);


	// ---
	

	// if not underwater and drawing water mode is on
	if(!get_under_water_level() && drawingWater) {
			draw_chunk(waterChunk, blockShaderProgram, worldAtlas, true, drawingWater);
	}


	// ---
	

	// draw crosshair
	draw_crosshair(window, worldAtlas);

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

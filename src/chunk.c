#include "../include/GLAD33/glad.h"
#include "../include/GLFW/glfw3.h"
#include "../include/CGLM/cglm.h"

#include <NOISE/noise1234.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/mesh.h"
#include "headers/image.h"
#include "headers/camera.h"
#include "headers/sky.h"

#include "headers/tex_coords.h"


// ---


// settings
const int CHUNK_WIDTH  = 32;  // x
const int CHUNK_HEIGHT = 32;  // y
const int CHUNK_LENGTH = 32;  // z

// noise settings
const int NOISE_ZOOM = 50;
const int NOISE_HEIGHT_OFFSET = 15;

// level at which stone blocks appear, coming from top
const int STONE_LEVEL = 5;

// level at which sand blocks appear, coming from bottom
const int SAND_LEVEL = 10;

// chance of a tree spawning on a grass block (ranges from 0.0f - 1.0f)
const float CHANCE_OF_TREE = 0.005f;

// maximum and minimum tree height in blocks
const int MAX_TREE_HEIGHT = 6;
const int MIN_TREE_HEIGHT = 5;

// trees can only be placed TREE_BUFFER_LEVEL amount of blocks below STONE_LEVEL
const int TREE_BUFFER_LEVEL = 4;

// water level (this is purely for rendering some stuff when under the water level)
const float CAM_WATER_LEVEL = SAND_LEVEL+0.5;

// boolean that checks if camera is under water
bool underWaterLevel = false;

// boolean that states whether or not to have fog on
bool showFog = true;

// random noise offset
float randomNoiseOffset = 0.0f;
// divide random noise offset value by this
const float RAND_NOISE_DIVIDER = 2000;


// ---


// takes an int and returns the string counterpart to that int block type
const char* int_to_string_block_type(int type) {
	// define type string based on returned block type
	
	switch(type) {
		case 1:
			return "grass";
			break;

		case 2:
			return "dirt";
			break;

		case 3:
			return "stone";
			break;

		case 4:
			return "sand";
			break;

		case 5:
			return "log";
			break;

		case 6:
			return "leaves";
			break;

		case 7:
			return "planks";
			break;

		case 8:
			return "bricks";
			break;

		case 9:
			return "diamond";
			break;

		case 10:
			return "smile";
			break;

		case 0:
			return "air";
			break;

		case -1:
			return "water";
			break;

		default:
			return NULL; // return NULL by default
	}

}


// ---


// toggles fog visibility
void toggle_fog() {
	showFog = !showFog;
}


// ---


// randomizes the noise offset
void randomize_noise_offset() {
	// set random noise offset
	randomNoiseOffset = rand() / RAND_NOISE_DIVIDER;
}


// ---


// getters for chunk sizes
int get_chunk_width() { 
	return CHUNK_WIDTH;
}
int get_chunk_length() {
	return CHUNK_LENGTH;
}
int get_chunk_height() {
	return CHUNK_HEIGHT;
}


// ---


// just returns the constant value of the camera water level float
float get_water_level() {
	return CAM_WATER_LEVEL;
}

// getter for under water-level
bool get_under_water_level() {
	return underWaterLevel;
}

// setter for under water-level
void set_under_water_level(bool value) {
	underWaterLevel = value;
}


// ---


// calculates noise value as integer block y coordinate at given position, allows for offsetting with chunk coords
int calc_chunk_noise_value(vec2 position, vec2 chunkOffset) {
	return (int) ( 
						noise2(
							(float) ( position[0] + chunkOffset[0]*CHUNK_WIDTH + randomNoiseOffset ) / NOISE_ZOOM, 
							(float) ( position[1] + chunkOffset[1]*CHUNK_LENGTH + randomNoiseOffset ) / NOISE_ZOOM
						) * CHUNK_HEIGHT
					 ) + NOISE_HEIGHT_OFFSET;
}


// ---


// chunk structure
struct Chunk {
	vec2 pos; // multiplied by CHUNK_WIDTH and CHUNK_LENGTH
	
	int sides; // how many sides do render (used when drawing it)
	
	int* blockTypes; // array of block types
	
	struct Mesh mesh; // mesh of chunk
};


// ---


// gets the block type at a position relative to the chunk based on coordinates
int get_block_type(int* blockTypes, int xPos, int yPos, int zPos) {
	return blockTypes[yPos*CHUNK_WIDTH*CHUNK_LENGTH + zPos*CHUNK_LENGTH + xPos];
}
// returns the actual index of a block at given relative coordinates to chunk
int get_block_index(int* blockTypes, int xPos, int yPos, int zPos) {
	return yPos*CHUNK_WIDTH*CHUNK_LENGTH + zPos*CHUNK_LENGTH + xPos;
}
// sets the block type at a position relative to the chunk based on coordinates
void set_block_type(int* blockTypes, int xPos, int yPos, int zPos, int type) {
	blockTypes[yPos*CHUNK_WIDTH*CHUNK_LENGTH + zPos*CHUNK_LENGTH + xPos] = type;
}


// ---


// creates a side vertices array from a template based on inputted side and coordinates, and writes it to given array
void create_side_vertices(const char* side, const char* blockType, int xPos, int yPos, int zPos, float* array) {

	// create initial coordinate values for vertices array
	float x1, y1, z1;
	float x2, y2, z2;
	float x3, y3, z3;
	float x4, y4, z4;

	// y position offset
	float yOff = 0;

	// declare and define color values (defaults are 1.0f)
	float r = 1.0f, g = 1.0f, b = 1.0f;

	// declare texture coordinates
	int tex_x1, tex_y1;
	int tex_x2, tex_y2;
	int tex_x3, tex_y3;
	int tex_x4, tex_y4;

	// texture offset for indexing a pre-made tex coords array
	int textureOffset;

	// declare texture coordinate values
	int texCoords[8*6];

	// load texture coordinates based on block type
	if(strcmp(blockType, "grass") == 0) {
		// copy grass texture coords array to tex_coords
		memcpy(texCoords, GRASS_TEX_COORDS, sizeof(int) * 8*6);
	}
	else if(strcmp(blockType, "dirt") == 0) {
		// copy dirt texture coords array to tex_coords
		memcpy(texCoords, DIRT_TEX_COORDS, sizeof(int) * 8*6);
	}
	else if(strcmp(blockType, "stone") == 0) {
		// copy stone texture coords array to tex_coords
		memcpy(texCoords, STONE_TEX_COORDS, sizeof(int) * 8*6);
	}
	else if(strcmp(blockType, "sand") == 0) {
		// copy sand texture coords array to tex_coords
		memcpy(texCoords, SAND_TEX_COORDS, sizeof(int) * 8*6);
	}
	else if(strcmp(blockType, "log") == 0) {
		// copy wood log texture coords array to tex_coords
		memcpy(texCoords, LOG_TEX_COORDS, sizeof(int) * 8*6);
	}
	else if(strcmp(blockType, "leaves") == 0) {
		// copy leaves texture coords array to tex_coords
		memcpy(texCoords, LEAVES_TEX_COORDS, sizeof(int) * 8*6);
	}
	else if(strcmp(blockType, "planks") == 0) {
		// copy planks texture coords array to tex_coords
		memcpy(texCoords, PLANKS_TEX_COORDS, sizeof(int) * 8*6);
	}
	else if(strcmp(blockType, "bricks") == 0) {
		// copy bricks texture coords array to tex_coords
		memcpy(texCoords, BRICKS_TEX_COORDS, sizeof(int) * 8*6);
	}
	else if(strcmp(blockType, "diamond") == 0) {
		// copy diamond texture coords array to tex_coords
		memcpy(texCoords, DIAMOND_TEX_COORDS, sizeof(int) * 8*6);
	}
	else if(strcmp(blockType, "smile") == 0) {
		// copy smile texture coords array to tex_coords
		memcpy(texCoords, SMILE_TEX_COORDS, sizeof(int) * 8*6);
	}
	else if(strcmp(blockType, "water") == 0) {
		// copy sand texture coords array to tex_coords
		memcpy(texCoords, WATER_TEX_COORDS, sizeof(int) * 8*6);
		
		yOff = 0.2f;
	}

	// compare side string to string literals and define coordinate floats, as well as set texture offset
	if(strcmp(side, "front") == 0) {
		x1 = 0.0f; y1 = 1.0f; z1 = 1.0f;
		x2 = 1.0f; y2 = 1.0f; z2 = 1.0f;
		x3 = 0.0f; y3 = 0.0f; z3 = 1.0f;
		x4 = 1.0f; y4 = 0.0f; z4 = 1.0f;

		r = 0.9f; g = 0.9f; b = 0.9f;

		textureOffset = 0;
	}
	else if(strcmp(side, "back") == 0) {
		x1 = 0.0f; y1 = 1.0f; z1 = 0.0f;
		x2 = 1.0f; y2 = 1.0f; z2 = 0.0f;
		x3 = 0.0f; y3 = 0.0f; z3 = 0.0f;
		x4 = 1.0f; y4 = 0.0f; z4 = 0.0f;

		r = 0.85f; g = 0.85f; b = 0.85f;

		textureOffset = 8;
	}
	else if(strcmp(side, "left") == 0) {
		x1 = 0.0f; y1 = 1.0f; z1 = 0.0f;
		x2 = 0.0f; y2 = 1.0f; z2 = 1.0f;
		x3 = 0.0f; y3 = 0.0f; z3 = 0.0f;
		x4 = 0.0f; y4 = 0.0f; z4 = 1.0f;

		r = 0.75f; g = 0.75f; b = 0.75f;

		textureOffset = 16;
	}
	else if(strcmp(side, "right") == 0) {
		x1 = 1.0f; y1 = 1.0f; z1 = 1.0f;
		x2 = 1.0f; y2 = 1.0f; z2 = 0.0f;
		x3 = 1.0f; y3 = 0.0f; z3 = 1.0f;
		x4 = 1.0f; y4 = 0.0f; z4 = 0.0f;

		r = 0.9f; g = 0.9f; b = 0.9f;

		textureOffset = 24;
	}
	else if(strcmp(side, "bottom") == 0) {
		x1 = 0.0f; y1 = 0.0f; z1 = 1.0f;
		x2 = 1.0f; y2 = 0.0f; z2 = 1.0f;
		x3 = 0.0f; y3 = 0.0f; z3 = 0.0f;
		x4 = 1.0f; y4 = 0.0f; z4 = 0.0f;

		r = 0.7f; g = 0.7f; b = 0.7f;

		textureOffset = 32;
	}
	else if(strcmp(side, "top") == 0) {
		x1 = 0.0f; y1 = 1.0f; z1 = 1.0f;
		x2 = 1.0f; y2 = 1.0f; z2 = 1.0f;
		x3 = 0.0f; y3 = 1.0f; z3 = 0.0f;
		x4 = 1.0f; y4 = 1.0f; z4 = 0.0f;

		textureOffset = 40;
	}

	// assign texture coordinates
	tex_x1 = texCoords[0+textureOffset]; tex_y1 = texCoords[1+textureOffset];
	tex_x2 = texCoords[2+textureOffset]; tex_y2 = texCoords[3+textureOffset];
	tex_x3 = texCoords[4+textureOffset]; tex_y3 = texCoords[5+textureOffset];
	tex_x4 = texCoords[6+textureOffset]; tex_y4 = texCoords[7+textureOffset];

	// generate vertices array
	float sideVertices[] = {
		// position                          color       texture coords

		x1+xPos,  y1+yPos-yOff,  z1+zPos,    r, g, b,    calc_at_tex_x(tex_x1), calc_at_tex_y(tex_y1),  // top left
		x2+xPos,  y2+yPos-yOff,  z2+zPos,    r, g, b,    calc_at_tex_x(tex_x2), calc_at_tex_y(tex_y2),  // top right
		x3+xPos,  y3+yPos-yOff,  z3+zPos,    r, g, b,    calc_at_tex_x(tex_x3), calc_at_tex_y(tex_y3),  // bot left
		x4+xPos,  y4+yPos-yOff,  z4+zPos,    r, g, b,    calc_at_tex_x(tex_x4), calc_at_tex_y(tex_y4),  // bot right
	};

	// copy contents of new sideVertices array into passed in array
	memcpy(array, sideVertices, sizeof(float) * 4*8);

}


// ---


// creates a side indices array from a template based on indices offset and index, and writes it to given array
void create_side_indices(int indicesOffset, int index, int* array) {

	// generate indices array
	int sideIndices[] = {
		indicesOffset+(index*24), indicesOffset+1+(index*24), indicesOffset+2+(index*24),
		indicesOffset+1+(index*24), indicesOffset+2+(index*24), indicesOffset+3+(index*24),
	};

	// copy contents of new sideIndices array into passed in array
	memcpy(array, sideIndices, sizeof(int) * 6);

}


// ---


void insert_block(struct Chunk* chunk, struct Chunk* leftChunk, struct Chunk* rightChunk, struct Chunk* topChunk, struct Chunk* bottomChunk, vec4 block) {

	// set the block type in the main chunk
	set_block_type((*chunk).blockTypes, block[0], block[1], block[2], block[3]);

	// bind vao
	glBindVertexArray((*chunk).mesh.vao);

	// bind vbo and ebo
	glBindBuffer(GL_ARRAY_BUFFER, (*chunk).mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*chunk).mesh.ebo);

	// define side vertices arrays for each side of a block
	float sideVertices[4*8];

	// create single side indices array
	int sideIndices[6];

	// declare type string
	const char* type;

	// convert type to string
	type = int_to_string_block_type(block[3]);


	int verticesIndex = 0;
	int indicesIndex = 0;
	int indicesOffset = 0;

	// get index of block
	int i = get_block_index((*chunk).blockTypes, block[0], block[1], block[2]);


	// ---


	// front

	// if not air block
	if(block[3] != 0) {
		// generate proper vertices array and load it into frontVertices
		create_side_vertices("front", type, block[0], block[1], block[2], sideVertices);
		
		// generate proper indices array and load it into sideIndices
		create_side_indices(indicesOffset, i, sideIndices);
	}

	// load proper vertices and indices array into VBO via glBufferSubData
	glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + verticesIndex, sizeof(sideVertices), sideVertices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + indicesIndex, sizeof(sideIndices), sideIndices);

	// increment vertices and indices index, as well as indices offset
	verticesIndex += 4*8 * sizeof(float);
	indicesIndex += 6 * sizeof(int);
	indicesOffset += 4;

	// increment amount of sides
	(*chunk).sides += 6;


	// ---
	

	// back
	
	// if not air block
	if(block[3] != 0) {
		// generate proper vertices array and load it into frontVertices
		create_side_vertices("back", type, block[0], block[1], block[2], sideVertices);
		
		// generate proper indices array and load it into sideIndices
		create_side_indices(indicesOffset, i, sideIndices);
	}

	// load proper vertices and indices array into VBO via glBufferSubData
	glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + verticesIndex, sizeof(sideVertices), sideVertices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + indicesIndex, sizeof(sideIndices), sideIndices);

	// increment vertices and indices index, as well as indices offset
	verticesIndex += 4*8 * sizeof(float);
	indicesIndex += 6 * sizeof(int);
	indicesOffset += 4;

	// increment amount of sides
	(*chunk).sides += 6;


	// ---
	

	// left

	// if not air block
	if(block[3] != 0) {
		// generate proper vertices array and load it into frontVertices
		create_side_vertices("left", type, block[0], block[1], block[2], sideVertices);
		
		// generate proper indices array and load it into sideIndices
		create_side_indices(indicesOffset, i, sideIndices);
	}

	// load proper vertices and indices array into VBO via glBufferSubData
	glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + verticesIndex, sizeof(sideVertices), sideVertices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + indicesIndex, sizeof(sideIndices), sideIndices);

	// increment vertices and indices index, as well as indices offset
	verticesIndex += 4*8 * sizeof(float);
	indicesIndex += 6 * sizeof(int);
	indicesOffset += 4;

	// increment amount of sides
	(*chunk).sides += 6;


	// ---
	

	// right

	// if not air block
	if(block[3] != 0) {
		// generate proper vertices array and load it into frontVertices
		create_side_vertices("right", type, block[0], block[1], block[2], sideVertices);
		
		// generate proper indices array and load it into sideIndices
		create_side_indices(indicesOffset, i, sideIndices);
	}

	// load proper vertices and indices array into VBO via glBufferSubData
	glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + verticesIndex, sizeof(sideVertices), sideVertices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + indicesIndex, sizeof(sideIndices), sideIndices);

	// increment vertices and indices index, as well as indices offset
	verticesIndex += 4*8 * sizeof(float);
	indicesIndex += 6 * sizeof(int);
	indicesOffset += 4;

	// increment amount of sides
	(*chunk).sides += 6;


	// ---
	

	// bottom

	// if not air block
	if(block[3] != 0) {
		// generate proper vertices array and load it into frontVertices
		create_side_vertices("bottom", type, block[0], block[1], block[2], sideVertices);
		
		// generate proper indices array and load it into sideIndices
		create_side_indices(indicesOffset, i, sideIndices);
	}

	// load proper vertices and indices array into VBO via glBufferSubData
	glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + verticesIndex, sizeof(sideVertices), sideVertices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + indicesIndex, sizeof(sideIndices), sideIndices);

	// increment vertices and indices index, as well as indices offset
	verticesIndex += 4*8 * sizeof(float);
	indicesIndex += 6 * sizeof(int);
	indicesOffset += 4;

	// increment amount of sides
	(*chunk).sides += 6;


	// ---
	

	// top

	// if not air block
	if(block[3] != 0) {
		// generate proper vertices array and load it into frontVertices
		create_side_vertices("top", type, block[0], block[1], block[2], sideVertices);
		
		// generate proper indices array and load it into sideIndices
		create_side_indices(indicesOffset, i, sideIndices);
	}

	// load proper vertices and indices array into VBO via glBufferSubData
	glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + verticesIndex, sizeof(sideVertices), sideVertices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + indicesIndex, sizeof(sideIndices), sideIndices);

	// increment amount of sides
	(*chunk).sides += 6;


	// ---
	

	// now we need to get adjacent blocks and fill in their sides, if new block is air
	if(block[3] == 0) {

		// the blocks are actually done kinda in reverse so the front block is actually the back block of
		// the main block being edited, and the right block is actually left to the main block, but its right side is being edited
		// which is why its being called the right block, i know its a bit confusing

		// front block

		// front position vector
		vec3 frontPos;
		glm_vec3_copy((vec3){block[0], block[1], block[2]-1}, frontPos);

		// if front block isnt air and position is within chunk
		if(get_block_type((*chunk).blockTypes, frontPos[0], frontPos[1], frontPos[2]) != 0 && frontPos[2] >= 0) {

			// get block type in the form of integer
			int blockType = get_block_type((*chunk).blockTypes, frontPos[0], frontPos[1], frontPos[2]);

			// get index of block
			int blockIndex = get_block_index((*chunk).blockTypes, frontPos[0], frontPos[1], frontPos[2]);

			// define type string based on returned block type
			type = int_to_string_block_type(blockType);


			// generate proper vertices array and load it into sideVertices
			create_side_vertices("front", type, frontPos[0], frontPos[1], frontPos[2], sideVertices);
			
			// generate proper indices array and load it into sideIndices
			create_side_indices( 0*(4) , blockIndex, sideIndices);

			// load proper vertices and indices array into VBO via glBufferSubData
			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * blockIndex + 0*(4*8*sizeof(float)), sizeof(sideVertices), sideVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * blockIndex + 0*(6*sizeof(int)), sizeof(sideIndices), sideIndices);

			// increment amount of sides
			(*chunk).sides += 6;

		}

		// back block

		// back position vector
		vec3 backPos;
		glm_vec3_copy((vec3){block[0], block[1], block[2]+1}, backPos);
		
		// if back block isnt air and position is within chunk
		if(get_block_type((*chunk).blockTypes, backPos[0], backPos[1], backPos[2]) != 0 && backPos[2] <= CHUNK_LENGTH-1) {

			// get block type in the form of integer
			int blockType = get_block_type((*chunk).blockTypes, backPos[0], backPos[1], backPos[2]);

			// get index of block
			int blockIndex = get_block_index((*chunk).blockTypes, backPos[0], backPos[1], backPos[2]);

			// define type string based on returned block type
			type = int_to_string_block_type(blockType);


			// generate proper vertices array and load it into sideVertices
			create_side_vertices("back", type, backPos[0], backPos[1], backPos[2], sideVertices);
			
			// generate proper indices array and load it into sideIndices
			create_side_indices( 1*(4) , blockIndex, sideIndices);

			// load proper vertices and indices array into VBO via glBufferSubData
			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * blockIndex + 1*(4*8*sizeof(float)), sizeof(sideVertices), sideVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * blockIndex + 1*(6*sizeof(int)), sizeof(sideIndices), sideIndices);

			// increment amount of sides
			(*chunk).sides += 6;

		}

		// left block

		// left position vector
		vec3 leftPos;
		glm_vec3_copy((vec3){block[0]+1, block[1], block[2]}, leftPos);
		
		// if left block isnt air
		if(get_block_type((*chunk).blockTypes, leftPos[0], leftPos[1], leftPos[2]) != 0 && leftPos[0] <= CHUNK_WIDTH-1) {

			// get block type in the form of integer
			int blockType = get_block_type((*chunk).blockTypes, leftPos[0], leftPos[1], leftPos[2]);

			// get index of block
			int blockIndex = get_block_index((*chunk).blockTypes, leftPos[0], leftPos[1], leftPos[2]);

			// define type string based on returned block type
			type = int_to_string_block_type(blockType);


			// generate proper vertices array and load it into sideVertices
			create_side_vertices("left", type, leftPos[0], leftPos[1], leftPos[2], sideVertices);
			
			// generate proper indices array and load it into sideIndices
			create_side_indices( 2*(4) , blockIndex, sideIndices);

			// load proper vertices and indices array into VBO via glBufferSubData
			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * blockIndex + 2*(4*8*sizeof(float)), sizeof(sideVertices), sideVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * blockIndex + 2*(6*sizeof(int)), sizeof(sideIndices), sideIndices);

			// increment amount of sides
			(*chunk).sides += 6;

		}

		// right block

		// right position vector
		vec3 rightPos;
		glm_vec3_copy((vec3){block[0]-1, block[1], block[2]}, rightPos);
		
		// if right block isnt air and position is within chunk
		if(get_block_type((*chunk).blockTypes, rightPos[0], rightPos[1], rightPos[2]) != 0 && rightPos[0] >= 0) {

			// get block type in the form of integer
			int blockType = get_block_type((*chunk).blockTypes, rightPos[0], rightPos[1], rightPos[2]);

			// get index of block
			int blockIndex = get_block_index((*chunk).blockTypes, rightPos[0], rightPos[1], rightPos[2]);

			// define type string based on returned block type
			type = int_to_string_block_type(blockType);


			// generate proper vertices array and load it into sideVertices
			create_side_vertices("right", type, rightPos[0], rightPos[1], rightPos[2], sideVertices);
			
			// generate proper indices array and load it into sideIndices
			create_side_indices( 3*(4) , blockIndex, sideIndices);

			// load proper vertices and indices array into VBO via glBufferSubData
			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * blockIndex + 3*(4*8*sizeof(float)), sizeof(sideVertices), sideVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * blockIndex + 3*(6*sizeof(int)), sizeof(sideIndices), sideIndices);

			// increment amount of sides
			(*chunk).sides += 6;

		}

		// bottom block

		// bottom position vector
		vec3 bottomPos;
		glm_vec3_copy((vec3){block[0], block[1]+1, block[2]}, bottomPos);
		
		// if bottom block isnt air and position is within chunk
		if(get_block_type((*chunk).blockTypes, bottomPos[0], bottomPos[1], bottomPos[2]) != 0) {

			// get block type in the form of integer
			int blockType = get_block_type((*chunk).blockTypes, bottomPos[0], bottomPos[1], bottomPos[2]);

			// get index of block
			int blockIndex = get_block_index((*chunk).blockTypes, bottomPos[0], bottomPos[1], bottomPos[2]);

			// define type string based on returned block type
			type = int_to_string_block_type(blockType);


			// generate proper vertices array and load it into sideVertices
			create_side_vertices("bottom", type, bottomPos[0], bottomPos[1], bottomPos[2], sideVertices);
			
			// generate proper indices array and load it into sideIndices
			create_side_indices( 4*(4) , blockIndex, sideIndices);

			// load proper vertices and indices array into VBO via glBufferSubData
			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * blockIndex + 4*(4*8*sizeof(float)), sizeof(sideVertices), sideVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * blockIndex + 4*(6*sizeof(int)), sizeof(sideIndices), sideIndices);

			// increment amount of sides
			(*chunk).sides += 6;

		}

		// top block

		// top position vector
		vec3 topPos;
		glm_vec3_copy((vec3){block[0], block[1]-1, block[2]}, topPos);
		
		// if top block isnt air
		if(get_block_type((*chunk).blockTypes, topPos[0], topPos[1], topPos[2]) != 0) {

			// get block type in the form of integer
			int blockType = get_block_type((*chunk).blockTypes, topPos[0], topPos[1], topPos[2]);

			// get index of block
			int blockIndex = get_block_index((*chunk).blockTypes, topPos[0], topPos[1], topPos[2]);

			// define type string based on returned block type
			type = int_to_string_block_type(blockType);


			// generate proper vertices array and load it into sideVertices
			create_side_vertices("top", type, topPos[0], topPos[1], topPos[2], sideVertices);
			
			// generate proper indices array and load it into sideIndices
			create_side_indices( 5*(4) , blockIndex, sideIndices);

			// load proper vertices and indices array into VBO via glBufferSubData
			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * blockIndex + 5*(4*8*sizeof(float)), sizeof(sideVertices), sideVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * blockIndex + 5*(6*sizeof(int)), sizeof(sideIndices), sideIndices);

			// increment amount of sides
			(*chunk).sides += 6;

		}

	}

	// unbind vbo and ebo
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



	// ---
	

	// bind vao
	glBindVertexArray((*chunk).mesh.vao);

	// bind vbo and ebo
	glBindBuffer(GL_ARRAY_BUFFER, (*chunk).mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*chunk).mesh.ebo);

	// vertex attributes
	
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// now unbind everything
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// ---
	

	// now we want to deal with the extra chunks if the operation was a deletion operation
	
	// if block being placed is air
	if(block[3] == 0) {

		// front block

		// front position vector
		vec3 frontPos;
		glm_vec3_copy((vec3){block[0], block[1], block[2]-1}, frontPos);

		// if front block position leaks into the side of the other chunk AND the chunk exists AND the block in that chunk isnt an air block (in this exact order)
		if( frontPos[2] == -1 && topChunk != NULL && get_block_type((*topChunk).blockTypes, frontPos[0], frontPos[1], CHUNK_LENGTH-1) != 0 ) {

			// bind vao
			glBindVertexArray((*topChunk).mesh.vao);

			// bind vbo and ebo
			glBindBuffer(GL_ARRAY_BUFFER, (*topChunk).mesh.vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*topChunk).mesh.ebo);


			// ---


			// get block type in the form of integer
			int blockType = get_block_type((*topChunk).blockTypes, frontPos[0], frontPos[1], CHUNK_LENGTH-1);

			// get index of block
			int blockIndex = get_block_index((*topChunk).blockTypes, frontPos[0], frontPos[1], CHUNK_LENGTH-1);

			// define type string based on returned block type
			type = int_to_string_block_type(blockType);


			// generate proper vertices array and load it into sideVertices
			create_side_vertices("front", type, frontPos[0], frontPos[1], CHUNK_LENGTH-1, sideVertices);
			
			// generate proper indices array and load it into sideIndices
			create_side_indices( 0*(4) , blockIndex, sideIndices);

			// load proper vertices and indices array into VBO via glBufferSubData
			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * blockIndex + 0*(4*8*sizeof(float)), sizeof(sideVertices), sideVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * blockIndex + 0*(6*sizeof(int)), sizeof(sideIndices), sideIndices);

			// increment amount of sides
			(*topChunk).sides += 6;

			
			// ---


			// vertex attributes
			
			// position
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			// color
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			// texture coords
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);

			// now unbind everything
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		}

		// back block

		// back position vector
		vec3 backPos;
		glm_vec3_copy((vec3){block[0], block[1], block[2]+1}, backPos);

		// if back block position leaks into the side of the other chunk AND the chunk exists AND the block in that chunk isnt an air block (in this exact order)
		if( backPos[2] == CHUNK_LENGTH && bottomChunk != NULL && get_block_type((*bottomChunk).blockTypes, backPos[0], backPos[1], 0) != 0 ) {

			// bind vao
			glBindVertexArray((*bottomChunk).mesh.vao);

			// bind vbo and ebo
			glBindBuffer(GL_ARRAY_BUFFER, (*bottomChunk).mesh.vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*bottomChunk).mesh.ebo);


			// ---


			// get block type in the form of integer
			int blockType = get_block_type((*bottomChunk).blockTypes, backPos[0], backPos[1], 0);

			// get index of block
			int blockIndex = get_block_index((*bottomChunk).blockTypes, backPos[0], backPos[1], 0);

			// define type string based on returned block type
			type = int_to_string_block_type(blockType);


			// generate proper vertices array and load it into sideVertices
			create_side_vertices("back", type, backPos[0], backPos[1], 0, sideVertices);
			
			// generate proper indices array and load it into sideIndices
			create_side_indices( 1*(4) , blockIndex, sideIndices);

			// load proper vertices and indices array into VBO via glBufferSubData
			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * blockIndex + 1*(4*8*sizeof(float)), sizeof(sideVertices), sideVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * blockIndex + 1*(6*sizeof(int)), sizeof(sideIndices), sideIndices);

			// increment amount of sides
			(*bottomChunk).sides += 6;

			
			// ---


			// vertex attributes
			
			// position
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			// color
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			// texture coords
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);

			// now unbind everything
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		}

		// left block

		// left position vector
		vec3 leftPos;
		glm_vec3_copy((vec3){block[0]-1, block[1], block[2]}, leftPos);

		// if left block position leaks into the side of the other chunk AND the chunk exists AND the block in that chunk isnt an air block (in this exact order)
		if( leftPos[0] == -1 && leftChunk != NULL && get_block_type((*leftChunk).blockTypes, CHUNK_WIDTH-1, leftPos[1], leftPos[2]) != 0 ) {

			// bind vao
			glBindVertexArray((*leftChunk).mesh.vao);

			// bind vbo and ebo
			glBindBuffer(GL_ARRAY_BUFFER, (*leftChunk).mesh.vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*leftChunk).mesh.ebo);


			// ---


			// get block type in the form of integer
			int blockType = get_block_type((*leftChunk).blockTypes, CHUNK_WIDTH-1, leftPos[1], leftPos[2]);

			// get index of block
			int blockIndex = get_block_index((*leftChunk).blockTypes, CHUNK_WIDTH-1, leftPos[1], leftPos[2]);

			// define type string based on returned block type
			type = int_to_string_block_type(blockType);


			// generate proper vertices array and load it into sideVertices
			create_side_vertices("right", type, CHUNK_WIDTH-1, leftPos[1], leftPos[2], sideVertices);
			
			// generate proper indices array and load it into sideIndices
			create_side_indices( 2*(4) , blockIndex, sideIndices);

			// load proper vertices and indices array into VBO via glBufferSubData
			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * blockIndex + 2*(4*8*sizeof(float)), sizeof(sideVertices), sideVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * blockIndex + 2*(6*sizeof(int)), sizeof(sideIndices), sideIndices);

			// increment amount of sides
			(*leftChunk).sides += 6;

			
			// ---


			// vertex attributes
			
			// position
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			// color
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			// texture coords
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);

			// now unbind everything
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		}

		// right block

		// right position vector
		vec3 rightPos;
		glm_vec3_copy((vec3){block[0]+1, block[1], block[2]}, rightPos);

		// if right block position leaks into the side of the other chunk AND the chunk exists AND the block in that chunk isnt an air block (in this exact order)
		if( rightPos[0] == CHUNK_WIDTH && rightChunk != NULL && get_block_type((*rightChunk).blockTypes, 0, rightPos[1], rightPos[2]) != 0 ) {

			// bind vao
			glBindVertexArray((*rightChunk).mesh.vao);

			// bind vbo and ebo
			glBindBuffer(GL_ARRAY_BUFFER, (*rightChunk).mesh.vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*rightChunk).mesh.ebo);


			// ---


			// get block type in the form of integer
			int blockType = get_block_type((*rightChunk).blockTypes, 0, rightPos[1], rightPos[2]);

			// get index of block
			int blockIndex = get_block_index((*rightChunk).blockTypes, 0, rightPos[1], rightPos[2]);

			// define type string based on returned block type
			type = int_to_string_block_type(blockType);


			// generate proper vertices array and load it into sideVertices
			create_side_vertices("left", type, 0, rightPos[1], rightPos[2], sideVertices);
			
			// generate proper indices array and load it into sideIndices
			create_side_indices( 3*(4) , blockIndex, sideIndices);

			// load proper vertices and indices array into VBO via glBufferSubData
			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * blockIndex + 3*(4*8*sizeof(float)), sizeof(sideVertices), sideVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * blockIndex + 3*(6*sizeof(int)), sizeof(sideIndices), sideIndices);

			// increment amount of sides
			(*rightChunk).sides += 6;

			
			// ---


			// vertex attributes
			
			// position
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			// color
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			// texture coords
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);

			// now unbind everything
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		}


	}

}


// ---


struct Chunk generate_chunk(vec2 position, int world_size, bool water) {

	// create new chunk structure instance
	struct Chunk newChunk;

	// allocate integer amount of maximum blocks possible in a chunk
	int blockAmount = CHUNK_WIDTH*CHUNK_HEIGHT*CHUNK_LENGTH;

	// if water then change the value of block amount
	if(water) {
		blockAmount = CHUNK_WIDTH*CHUNK_HEIGHT * world_size*world_size;
	}

	// x and y pos for placing blocks
	int xPos = 0;
	int yPos = 0;
	int zPos = 0;

	// allocate memory to block amount to be used as an array
	newChunk.blockTypes = calloc(blockAmount, sizeof(int));

	// set sides to 0
	newChunk.sides = 0;

	
	// ---
	

	// create vbo
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	// bind vbo
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (6*4*8 * blockAmount), 0, GL_STATIC_DRAW);


	// ---
	

	// create ebo
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	// bind ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * (6*6 * blockAmount), 0, GL_STATIC_DRAW);


	// ---


	// if its not a water chunk
	if(!water) {


		int noiseValue = 0;

		// first iteration, load all coordinates of blocks, as this allows for later optimization
		for(int i=0; i < blockAmount; i++) {

			// calculate noise value


			noiseValue = calc_chunk_noise_value((vec2){xPos, zPos}, position);
			
			// cap noise value to chunk height
			if(noiseValue >= CHUNK_HEIGHT) {
				noiseValue = CHUNK_HEIGHT-1;
			}

			// based on noise value, fill with blocks or air
			if(yPos > CHUNK_HEIGHT-STONE_LEVEL && yPos <= noiseValue) {
				newChunk.blockTypes[i] = 3; // stone
			}
			else if(yPos <= SAND_LEVEL && yPos <= noiseValue) {
				newChunk.blockTypes[i] = 4; // sand
			}
			else if(yPos == noiseValue+1 
					&& yPos < CHUNK_HEIGHT-STONE_LEVEL-TREE_BUFFER_LEVEL && yPos > SAND_LEVEL+1
					&& xPos > 0 && xPos < CHUNK_WIDTH-1
					&& zPos > 0 && zPos < CHUNK_LENGTH-1) {
				// tree generation

				// random value in between 0-1, determines if a tree will be placed
				float randomValue = (float)rand() / RAND_MAX;

				// if value is less than the chance to spawn a tree
				if(randomValue < CHANCE_OF_TREE) {
					newChunk.blockTypes[i] = 5; // first log block, later used to generate the trees
				}
				else {
					newChunk.blockTypes[i] = 0; // air
				}
			}
			else if(yPos == noiseValue) {
				newChunk.blockTypes[i] = 1; // grass
			}
			else if(yPos < noiseValue) {
				newChunk.blockTypes[i] = 2; // dirt
			}
			else {
				newChunk.blockTypes[i] = 0; // air
			}

			// if bottom most layer, then fill it in automatically
			if(yPos == 0) {
				newChunk.blockTypes[i] = 4; // sand
			}
			

			// ---


			xPos++;
		
			// if x position exceeds 16th block then reset it and increment z position
			if(xPos >= CHUNK_WIDTH) {
				zPos++;
				xPos = 0;
			}

			// if z position exceeds 16th block then reset it (as well as x) and increment y position
			if(zPos >= CHUNK_LENGTH) {
				yPos++;
				zPos = 0;
				xPos = 0;
			}

		}


		// ---
		

		// reset positions
		xPos = 0;
		yPos = 0;
		zPos = 0;

		
		// ---
		

		// another for loop, find all the start blocks for trees and expand on them
		for(int i=0; i < blockAmount; i++) {

			// if the block is a wood log and the block below it is grass (meaning its the first log block of the tree)
			if(newChunk.blockTypes[i] == 5 && get_block_type(newChunk.blockTypes, xPos, yPos-1, zPos) == 1) {

				// get amount of log blocks to expand upon for blocks (remove 1 cus we already start with one)
				int logAmount = ( (float)rand() / (float)(RAND_MAX) ) * (MAX_TREE_HEIGHT-MIN_TREE_HEIGHT) + MIN_TREE_HEIGHT - 1;

				// if indexed y position is below half of the stone level (trying to prevent segfaults by indexing out of blockTypes)
				if(yPos+logAmount > CHUNK_HEIGHT-STONE_LEVEL) {
					// cap logAmount at that value
					logAmount = CHUNK_HEIGHT-STONE_LEVEL - yPos;
				}

				// iterate thru log amount
				for(int l=0; l < logAmount; l++) {

					// get index of log block
					int logIndex = get_block_index(newChunk.blockTypes, xPos, yPos+l, zPos);

					// set that indexed block to a log block
					newChunk.blockTypes[logIndex] = 5;

				}

				
				// ---


				// now leaves blocks

				// top (100% spawn)
				newChunk.blockTypes[get_block_index(newChunk.blockTypes, xPos, yPos+logAmount, zPos)] = 6;

				// left (100% spawn)
				newChunk.blockTypes[get_block_index(newChunk.blockTypes, xPos-1, yPos+logAmount-1, zPos)] = 6;

				// right (100% spawn)
				newChunk.blockTypes[get_block_index(newChunk.blockTypes, xPos+1, yPos+logAmount-1, zPos)] = 6;

				// back (100% spawn)
				newChunk.blockTypes[get_block_index(newChunk.blockTypes, xPos, yPos+logAmount-1, zPos-1)] = 6;

				// front (100% spawn)
				newChunk.blockTypes[get_block_index(newChunk.blockTypes, xPos, yPos+logAmount-1, zPos+1)] = 6;

				// random amount of extra blocks (12 extra spots if u count it)
				int extraLeaves = (int)( ((float)rand() / (float)RAND_MAX) * 12 );

				for(int l=0; l < extraLeaves; l++) {
					// x can be from -1 - 1
					int x =  floor( ((float)rand() / (float)RAND_MAX) * 3  - 1);

					// y can be either 0 or 1
					int y =  floor( ((float)rand() / (float)RAND_MAX) * 2 );

					// z can be from -1 - 1
					int z =  floor( ((float)rand() / (float)RAND_MAX) * 3  - 1);

					// insert this new leaves block
					newChunk.blockTypes[get_block_index(newChunk.blockTypes, xPos+x, yPos+logAmount-1+y, zPos+z)] = 6;

				}

			}

			// ---


			xPos++;
		
			// if x position exceeds 16th block then reset it and increment z position
			if(xPos >= CHUNK_WIDTH) {
				zPos++;
				xPos = 0;
			}

			// if z position exceeds 16th block then reset it (as well as x) and increment y position
			if(zPos >= CHUNK_LENGTH) {
				yPos++;
				zPos = 0;
				xPos = 0;
			}

		}

		// ---
		

		// reset positions
		xPos = 0;
		yPos = 0;
		zPos = 0;


		// ---
		

		// define side vertices arrays for each side of a block
		float frontVertices  [4*8];
		float backVertices   [4*8];
		float leftVertices   [4*8];
		float rightVertices  [4*8];
		float bottomVertices [4*8];
		float topVertices    [4*8];

		// create single side indices array
		int sideIndices[6];

		// now iterate (again) thru all block positions
		for(int i=0; i < blockAmount; i++) {

			// add on vertices index
			int verticesIndex = 0;

			int indicesIndex = 0;
			int indicesOffset = 0;


			// ---
		

			// booleans to tell whether to add those sides or not
			bool front  = true;
			bool back   = true;
			bool left   = true;
			bool right  = true;
			bool bottom = true;
			bool top    = true;

			// set all to false if block type is 0 (air)
			if(get_block_type(newChunk.blockTypes, xPos, yPos, zPos) == 0) {
				front  = false;
				back   = false;
				left   = false;
				right  = false;
				bottom = false;
				top    = false;
			}
					
			// front
			if( zPos != CHUNK_LENGTH-1 && get_block_type(newChunk.blockTypes, xPos, yPos, zPos+1) > 0 ) {
				front = false;
			}
			// back
			if( zPos != 0 && get_block_type(newChunk.blockTypes, xPos, yPos, zPos-1) > 0 ) {
				back = false;
			}
			// left
			if( xPos != 0 && get_block_type(newChunk.blockTypes, xPos-1, yPos, zPos) > 0 ) {
				left = false;
			}
			// right
			if( xPos != CHUNK_WIDTH-1 && get_block_type(newChunk.blockTypes, xPos+1, yPos, zPos) > 0 ) {
				right = false;
			}
			// bottom (remove bottom face if it is the bottom-most block too)
			if( (yPos != 0 && get_block_type(newChunk.blockTypes, xPos, yPos-1, zPos) > 0)
					|| yPos == 0) {
				bottom = false;
			}
			// top
			if( yPos != CHUNK_HEIGHT-1 && get_block_type(newChunk.blockTypes, xPos, yPos+1, zPos) > 0 ) {
				top = false;
			}

			// declare type string
			const char* type;

			// get string counterpart from int format of block type
			type = int_to_string_block_type(
					get_block_type(newChunk.blockTypes, xPos, yPos, zPos)
			);


			// only load vertices and indices for necessary sides
			

			if(front) {
				// generate proper vertices array and load it into frontVertices
				create_side_vertices("front", type, xPos, yPos, zPos, frontVertices);
				
				// generate proper indices array and load it into sideIndices
				create_side_indices(indicesOffset, i, sideIndices);

				// load proper vertices and indices array into VBO via glBufferSubData
				glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + 0*(4*8 * sizeof(float)), sizeof(frontVertices), frontVertices);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + 0*(6 * sizeof(int)), sizeof(sideIndices), sideIndices);
			}
			// increment vertices and indices index, as well as indices offset
			verticesIndex += 4*8 * sizeof(float);
			indicesIndex += 6 * sizeof(int);
			indicesOffset += 4;

			// increment amount of sides
			newChunk.sides += 6;

			if(back) {
				// generate proper vertices array and load it into backVertices
				create_side_vertices("back", type, xPos, yPos, zPos, backVertices);

				// generate proper indices array and load it into sideIndices
				create_side_indices(indicesOffset, i, sideIndices);

				// load proper vertices and indices array into VBO via glBufferSubData
				glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + 1*(4*8 * sizeof(float)), sizeof(backVertices), backVertices);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + 1*(6 * sizeof(int)), sizeof(sideIndices), sideIndices);

				// increment amount of sides
				newChunk.sides += 6;
			}
			// increment vertices and indices index, as well as indices offset
			verticesIndex += 4*8 * sizeof(float);
			indicesIndex += 6 * sizeof(int);
			indicesOffset += 4;

			if(left) {
				// generate proper vertices array and load it into leftVertices
				create_side_vertices("left", type,  xPos, yPos, zPos, leftVertices);

				// generate proper indices array and load it into sideIndices
				create_side_indices(indicesOffset, i, sideIndices);

				// load proper vertices and indices array into VBO via glBufferSubData
				glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + 2*(4*8 * sizeof(float)), sizeof(leftVertices), leftVertices);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + 2*(6 * sizeof(int)), sizeof(sideIndices), sideIndices);

				// increment amount of sides
				newChunk.sides += 6;
			}
			// increment vertices and indices index, as well as indices offset
			verticesIndex += 4*8 * sizeof(float);
			indicesIndex += 6 * sizeof(int);
			indicesOffset += 4;

			if(right) {
				// generate proper vertices array and load it into rightVertices
				create_side_vertices("right", type, xPos, yPos, zPos, rightVertices);

				// generate proper indices array and load it into sideIndices
				create_side_indices(indicesOffset, i, sideIndices);

				// load proper vertices and indices array into VBO via glBufferSubData
				glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + 3*(4*8 * sizeof(float)), sizeof(rightVertices), rightVertices);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + 3*(6 * sizeof(int)), sizeof(sideIndices), sideIndices);

				// increment amount of sides
				newChunk.sides += 6;
			}
			// increment vertices and indices index, as well as indices offset
			verticesIndex += 4*8 * sizeof(float);
			indicesIndex += 6 * sizeof(int);
			indicesOffset += 4;

			if(bottom && strcmp(type, "water") != 0) {
				// generate proper vertices array and load it into bottomVertices
				create_side_vertices("bottom", type, xPos, yPos, zPos, bottomVertices);

				// generate proper indices array and load it into sideIndices
				create_side_indices(indicesOffset, i, sideIndices);

				// load proper vertices and indices array into VBO via glBufferSubData
				glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + 4*(4*8 * sizeof(float)), sizeof(bottomVertices), bottomVertices);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + 4*(6 * sizeof(int)), sizeof(sideIndices), sideIndices);

				// increment amount of sides
				newChunk.sides += 6;
			}
			// increment vertices and indices index, as well as indices offset
			verticesIndex += 4*8 * sizeof(float);
			indicesIndex += 6 * sizeof(int);
			indicesOffset += 4;

			if(top) {
				// generate proper vertices array and load it into topVertices
				create_side_vertices("top", type, xPos, yPos, zPos, topVertices);

				// generate proper indices array and load it into sideIndices
				create_side_indices(indicesOffset, i, sideIndices);

				// load proper vertices and indices array into VBO via glBufferSubData
				glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + 5*(4*8 * sizeof(float)), sizeof(topVertices), topVertices);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + 5*(6 * sizeof(int)), sizeof(sideIndices), sideIndices);

				// increment amount of sides
				newChunk.sides += 6;
			}


			// ---


			xPos++;
		
			// if x position exceeds 16th block then reset it and increment z position
			if(xPos >= CHUNK_WIDTH) {
				zPos++;
				xPos = 0;
			}

			// if z position exceeds 16th block then reset it (as well as x) and increment y position
			if(zPos >= CHUNK_LENGTH) {
				yPos++;
				zPos = 0;
				xPos = 0;
			}

		}


	}
	else { // if its a water chunk

		// chunk x and y position
		int chunkX = 0;
		int chunkY = 0;

		// create single side indices array
		//int sideIndices[6];

		// top vertices array
		float topVertices[4*8];

		// offset for indices
		int indicesOffset = 0;

		// x y z values for water face
		int x = 0;
		int y = SAND_LEVEL-1;
		int z = 0;

		// iterate through all water blocks/faces thingies in the world
		for(int b=0; b < CHUNK_LENGTH*CHUNK_WIDTH*world_size*world_size; b++) {


			// generate proper vertices array and load it into topVertices
			create_side_vertices("top", "water", x, y, z, topVertices);

			// generate custom indices array
			int sideIndices[] = {
				indicesOffset, indicesOffset+1, indicesOffset+2,
				indicesOffset+1, indicesOffset+2, indicesOffset+3,
			};

			// load proper vertices and indices array into VBO via glBufferSubData
			glBufferSubData(GL_ARRAY_BUFFER, 
					b * ( sizeof(float) * 4*8 ), 
					sizeof(topVertices), topVertices);

			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 
					b * ( sizeof(int) * 6 ),
					sizeof(sideIndices), sideIndices);

			// increment amount of sides
			newChunk.sides += 6;

			// increment indices offset
			indicesOffset += 4;


			// ---

			
			// position handling

			x++;

			if(x >= world_size*CHUNK_WIDTH) {
				x = 0;
				z++;
			}

		}




	}
	

	// unbind vbo and ebo
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// ---
	

	// create vao
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	// bind vao
	glBindVertexArray(VAO);

	// bind vbo and ebo
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	// vertex attributes
	
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// unbind vao
	glBindVertexArray(0);

	// unbind vbo and ebo
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// ---
	

	// create chunk mesh object
	struct Mesh chunkMesh;

	// assign vao, vbo, and ebo to mesh
	chunkMesh.vao = VAO;
	chunkMesh.vbo = VBO;
	chunkMesh.ebo = EBO;

	// apply mesh to chunk object
	newChunk.mesh = chunkMesh;

	// apply position to chunk object
	glm_vec2_copy(position, newChunk.pos);

	// return newly generated chunk object
	return newChunk;

}


// ---


// optimize the chunks to remove internal faces
void handle_chunk_sides(struct Chunk* chunk, struct Chunk* leftChunk, struct Chunk* rightChunk, struct Chunk* topChunk, struct Chunk* bottomChunk) {

	// bind vao
	glBindVertexArray((*chunk).mesh.vao);

	// bind vbo and ebo
	glBindBuffer(GL_ARRAY_BUFFER, (*chunk).mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*chunk).mesh.ebo);

	// create empty vertices array for that side
	float sideVertices[4*8];

	// create empty indices array for that side
	float sideIndices[6];

	// check left chunk, main chunk x = 0, left chunk x = CHUNK_WIDTH-1

	// if passed left chunk is not NULL
	if(leftChunk != NULL) {
		// iterate thru z and y
		for(int z = 0; z < CHUNK_LENGTH; z++) {

			for(int y = 0; y < CHUNK_HEIGHT; y++) {
			
				// get type of corresponding block in main chunk on its left side
				int mainBlockType = get_block_type((*chunk).blockTypes, 0, y, z);

				// get type of corresponding block in left chunk on its right side (so to the left of the indexed main block)
				int leftBlockType = get_block_type((*leftChunk).blockTypes, CHUNK_WIDTH-1, y, z);

				// if the block on the left isn't air and main block also isn't air
				if(leftBlockType != 0 && mainBlockType != 0) {

					// we want to remove that side from the buffer for now
					
					// get the index of that block
					int i = get_block_index((*chunk).blockTypes, 0, y, z);

					// load proper vertices and indices array into VBO via glBufferSubData
					glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + 2*(4*8*sizeof(float)), sizeof(sideVertices), sideVertices);
					glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + 2*(6*sizeof(int)), sizeof(sideIndices), sideIndices);
				}

			}

		}
	}
	// otherwise just remove the whole side facing edge of world
	else {
		// iterate thru z and y
		for(int z = 0; z < CHUNK_LENGTH; z++) {

			for(int y = 0; y < CHUNK_HEIGHT; y++) {
				
				// get the index of that block
				int i = get_block_index((*chunk).blockTypes, 0, y, z);

				// load proper vertices and indices array into VBO via glBufferSubData
				glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + 2*(4*8*sizeof(float)), sizeof(sideVertices), sideVertices);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + 2*(6*sizeof(int)), sizeof(sideIndices), sideIndices);

			}

		}
	}


	// ---
	

	// check right chunk, main chunk x = CHUNK_WIDTH-1, right chunk x = 0
	
	// if passed right chunk is not NULL
	if(rightChunk != NULL) {
		// iterate thru z and y
		for(int z = 0; z < CHUNK_LENGTH; z++) {

			for(int y = 0; y < CHUNK_HEIGHT; y++) {
			
				// get type of corresponding block in main chunk on its right side
				int mainBlockType = get_block_type((*chunk).blockTypes, CHUNK_WIDTH-1, y, z);

				// get type of corresponding block in right chunk on its left side (so to the right of the indexed main block)
				int rightBlockType = get_block_type((*rightChunk).blockTypes, 0, y, z);

				// if the block on the right isn't air and main block also isn't air
				if(rightBlockType != 0 && mainBlockType != 0) {

					// we want to remove that side from the buffer for now
					
					// get the index of that block
					int i = get_block_index((*chunk).blockTypes, CHUNK_WIDTH-1, y, z);

					// load proper vertices and indices array into VBO via glBufferSubData
					glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + 3*(4*8*sizeof(float)), sizeof(sideVertices), sideVertices);
					glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + 3*(6*sizeof(int)), sizeof(sideIndices), sideIndices);

				}

			}

		}
	}
	// otherwise just remove the whole side facing edge of world
	else {
		// iterate thru z and y
		for(int z = 0; z < CHUNK_LENGTH; z++) {

			for(int y = 0; y < CHUNK_HEIGHT; y++) {
				
				// get the index of that block
				int i = get_block_index((*chunk).blockTypes, CHUNK_WIDTH-1, y, z);

				// load proper vertices and indices array into VBO via glBufferSubData
				glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + 3*(4*8*sizeof(float)), sizeof(sideVertices), sideVertices);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + 3*(6*sizeof(int)), sizeof(sideIndices), sideIndices);

			}

		}
	}


	// ---
	

	// check top chunk, main chunk z = 0, top chunk z = CHUNK_LENGTH-1

	// if passed top chunk is not NULL
	if(topChunk != NULL) {
		// iterate thru z and y
		for(int x = 0; x < CHUNK_WIDTH; x++) {

			for(int y = 0; y < CHUNK_HEIGHT; y++) {
			
				// get type of corresponding block in main chunk on its top side
				int mainBlockType = get_block_type((*chunk).blockTypes, x, y, CHUNK_LENGTH-1);

				// get type of corresponding block in top chunk on its bottom (front) side (so behind the indexed main block)
				int topBlockType = get_block_type((*topChunk).blockTypes, x, y, 0);

				// if the block at top (back) isn't air and main block also isn't air
				if(topBlockType != 0 && mainBlockType != 0) {

					// we want to remove that side from the buffer for now
					
					// get the index of that block
					int i = get_block_index((*chunk).blockTypes, x, y, CHUNK_LENGTH-1);

					// load proper vertices and indices array into VBO via glBufferSubData
					glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + 0*(4*8*sizeof(float)), sizeof(sideVertices), sideVertices);
					glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + 0*(6*sizeof(int)), sizeof(sideIndices), sideIndices);

				}

			}

		}
	}
	// otherwise just remove the whole side facing edge of world
	else {
		// iterate thru z and y
		for(int x = 0; x < CHUNK_WIDTH; x++) {

			for(int y = 0; y < CHUNK_HEIGHT; y++) {
				
				// get the index of that block
				int i = get_block_index((*chunk).blockTypes, x, y, CHUNK_LENGTH-1);

				// load proper vertices and indices array into VBO via glBufferSubData
				glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + 0*(4*8*sizeof(float)), sizeof(sideVertices), sideVertices);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + 0*(6*sizeof(int)), sizeof(sideIndices), sideIndices);

			}

		}
	}


	// ---
	
	
	// check bottom chunk, main chunk z = CHUNK_LENGTH-1, bottom chunk z = 0

	// if passed bottom chunk is not NULL
	if(bottomChunk != NULL) {
		// iterate thru z and y
		for(int x = 0; x < CHUNK_WIDTH; x++) {

			for(int y = 0; y < CHUNK_HEIGHT; y++) {
			
				// get type of corresponding block in main chunk on its bottom side
				int mainBlockType = get_block_type((*chunk).blockTypes, x, y, 0);

				// get type of corresponding block in bottom chunk on its top (back) side (so in front of the indexed main block)
				int bottomBlockType = get_block_type((*bottomChunk).blockTypes, x, y, CHUNK_LENGTH-1);

				// if the block at bottom (front) isn't air and main block also isn't air
				if(bottomBlockType != 0 && mainBlockType != 0) {

					// we want to remove that side from the buffer for now
					
					// get the index of that block
					int i = get_block_index((*chunk).blockTypes, x, y, 0);

					// load proper vertices and indices array into VBO via glBufferSubData
					glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + 1*(4*8*sizeof(float)), sizeof(sideVertices), sideVertices);
					glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + 1*(6*sizeof(int)), sizeof(sideIndices), sideIndices);

				}

			}

		}
	}
	// otherwise just remove the whole side facing edge of world
	else {
		// iterate thru z and y
		for(int x = 0; x < CHUNK_WIDTH; x++) {

			for(int y = 0; y < CHUNK_HEIGHT; y++) {
				
				// get the index of that block
				int i = get_block_index((*chunk).blockTypes, x, y, 0);

				// load proper vertices and indices array into VBO via glBufferSubData
				glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + 1*(4*8*sizeof(float)), sizeof(sideVertices), sideVertices);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + 1*(6*sizeof(int)), sizeof(sideIndices), sideIndices);

			}

		}
	}


	// ---


	// unbind vbo and ebo
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// ---
	

	// bind vao
	glBindVertexArray((*chunk).mesh.vao);

	// bind vbo and ebo
	glBindBuffer(GL_ARRAY_BUFFER, (*chunk).mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*chunk).mesh.ebo);

	// vertex attributes
	
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// now unbind everything
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}


// ---


void draw_chunk(struct Chunk chunk, unsigned int shaderProgram, unsigned int worldAtlas, bool water, bool drawingWater) {

	// bind vao
	glBindVertexArray(chunk.mesh.vao);

	// use shader program
	glUseProgram(shaderProgram);

	// bind texture
	glBindTexture(GL_TEXTURE_2D, worldAtlas);

	// get camera matrices
	mat4* model = get_model();
	mat4* view = get_view();
	mat4* proj = get_projection();

	// get camera position
	vec3* camPos = get_camera_pos();

	// get block shading
	float shading = get_block_shading();

	// get locations of uniform camera matrices
	int modelLoc = glGetUniformLocation(shaderProgram, "model");
	int viewLoc = glGetUniformLocation(shaderProgram, "view");
	int projLoc = glGetUniformLocation(shaderProgram, "proj");

	// get location of chunk offset uniform
	int posLoc = glGetUniformLocation(shaderProgram, "chunkOffset");

	// get location of underwater uniform
	int uwLoc = glGetUniformLocation(shaderProgram, "underWater");

	// get location of uniform shading float
	int shadingLoc = glGetUniformLocation(shaderProgram, "shading");

	// get location of uniform tide float
	int tideLoc = glGetUniformLocation(shaderProgram, "tide");

	// get location of camera position uniform vector
	int camPosLoc = glGetUniformLocation(shaderProgram, "camPos");

	// get location of uniform boolean for showing fog
	int fogLoc = glGetUniformLocation(shaderProgram, "fog");

	// load data into uniforms
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, *model);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *view);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, *proj);

	// load chunk position/offset into corresponding uniform vector
	glUniform2f(posLoc, chunk.pos[0]*CHUNK_WIDTH, chunk.pos[1]*CHUNK_LENGTH);

	// pass underWaterLevel boolean in the form of an integer to fragment shader
	if(underWaterLevel && drawingWater) {
		glUniform1i(uwLoc, 1);
	}
	else {
		glUniform1i(uwLoc, 0);
	}

	// pass block shading uniform to fragment shader
	glUniform1f(shadingLoc, shading);

	// pass tide level to vertex shader based on whether its a water chunk or not
	if(water) {
		glUniform1f(tideLoc, get_tide_level());
	}
	else {
		glUniform1f(tideLoc, 0);
	}

	// pass camera position as uniform vector3 to vertex shader
	glUniform3f(camPosLoc, (*camPos)[0], (*camPos)[1], (*camPos)[2]);

	// pass showFog boolean as uniform int to vertex shader
	glUniform1i(fogLoc, showFog);

	// draw the elements
	glDrawElements(GL_TRIANGLES, 36 * chunk.sides, GL_UNSIGNED_INT, 0);

}

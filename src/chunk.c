#include <GLAD33/glad.h>
#include <GLFW/glfw3.h>
#include <CGLM/cglm.h>

#include <stdio.h>
#include <string.h>

#include "headers/mesh.h"
#include "headers/image.h"
#include "headers/camera.h"


// ---


// settings
const int CHUNK_HEIGHT = 32;


// ---


// chunk structure
struct Chunk {
	vec2 pos; // multiplied by 16
	
	int blockCount; // amount of blocks
	
	// block positions
	vec4* blockPositions;
	
	struct Mesh mesh; // mesh of chunk
};

// ---


// grass block texture coordinates
int GRASS_TEX_COORDS[] = {
	// front
	0,  0,
	16, 0,
	0,  16,
	16, 16,

	// back
	16, 0,
	0,  0,
	16, 16,
	0,  16,

	// left
	0,  0,
	16, 0,
	0,  16,
	16, 16,

	// right
	0,  0,
	16, 0,
	0,  16,
	16, 16,

	// bottom
	16, 0,
	32, 0,
	16, 16,
	32, 16,

	// top
	32, 0,
	48, 0,
	32, 16,
	48, 16,
};


// ---


// creates a side vertices array from a template based on inputted side and coordinates, and writes it to given array
void create_side_vertices(const char* side, const char* blockType, int xPos, int yPos, int zPos, float* array) {

	// create initial coordinate values for vertices array
	float x1, y1, z1;
	float x2, y2, z2;
	float x3, y3, z3;
	float x4, y4, z4;

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

	// compare side string to string literals and define coordinate floats, as well as set texture offset
	if(strcmp(side, "front") == 0) {
		x1 = 0.0f; y1 = 1.0f; z1 = 1.0f;
		x2 = 1.0f; y2 = 1.0f; z2 = 1.0f;
		x3 = 0.0f; y3 = 0.0f; z3 = 1.0f;
		x4 = 1.0f; y4 = 0.0f; z4 = 1.0f;

		textureOffset = 0;
	}
	else if(strcmp(side, "back") == 0) {
		x1 = 0.0f; y1 = 1.0f; z1 = 0.0f;
		x2 = 1.0f; y2 = 1.0f; z2 = 0.0f;
		x3 = 0.0f; y3 = 0.0f; z3 = 0.0f;
		x4 = 1.0f; y4 = 0.0f; z4 = 0.0f;

		textureOffset = 8;
	}
	else if(strcmp(side, "left") == 0) {
		x1 = 0.0f; y1 = 1.0f; z1 = 0.0f;
		x2 = 0.0f; y2 = 1.0f; z2 = 1.0f;
		x3 = 0.0f; y3 = 0.0f; z3 = 0.0f;
		x4 = 0.0f; y4 = 0.0f; z4 = 1.0f;

		textureOffset = 16;
	}
	else if(strcmp(side, "right") == 0) {
		x1 = 1.0f; y1 = 1.0f; z1 = 1.0f;
		x2 = 1.0f; y2 = 1.0f; z2 = 0.0f;
		x3 = 1.0f; y3 = 0.0f; z3 = 1.0f;
		x4 = 1.0f; y4 = 0.0f; z4 = 0.0f;

		textureOffset = 24;
	}
	else if(strcmp(side, "bottom") == 0) {
		x1 = 0.0f; y1 = 0.0f; z1 = 1.0f;
		x2 = 1.0f; y2 = 0.0f; z2 = 1.0f;
		x3 = 0.0f; y3 = 0.0f; z3 = 0.0f;
		x4 = 1.0f; y4 = 0.0f; z4 = 0.0f;

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
		// position                     color                texture coords

		x1+xPos,  y1+yPos,  z1+zPos,    1.0f, 0.0f, 0.0f,    calc_at_tex_x(tex_x1), calc_at_tex_y(tex_y1),  // top left
		x2+xPos,  y2+yPos,  z2+zPos,    0.0f, 1.0f, 0.0f,    calc_at_tex_x(tex_x2), calc_at_tex_y(tex_y2),  // top right
		x3+xPos,  y3+yPos,  z3+zPos,    0.0f, 0.0f, 1.0f,    calc_at_tex_x(tex_x3), calc_at_tex_y(tex_y3),  // bot left
		x4+xPos,  y4+yPos,  z4+zPos,    1.0f, 0.0f, 1.0f,    calc_at_tex_x(tex_x4), calc_at_tex_y(tex_y4),  // bot right
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


struct Chunk generate_chunk() {

	// create new chunk structure instance
	struct Chunk newChunk;

	// allocate integer amount of maximum blocks possible in a chunk
	int blockAmount = 16*16*CHUNK_HEIGHT;

	// x and y pos for placing blocks
	int xPos = 0;
	int yPos = 0;
	int zPos = 0;

	// allocate memory to block amount to be used as an array
	newChunk.blockPositions = calloc(sizeof(vec4), blockAmount);

	
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
	

	// keep track of blocks
	int blockCount = 0;

	// first iteration, load all coordinates of blocks, as this allows for later optimization
	for(int i=0; i < blockAmount; i++) {
		// write position vector of current block to blockpositions array
		glm_vec4_copy((vec4){xPos, yPos, zPos, 1}, newChunk.blockPositions[i]);


		// ---


		xPos++;
	
		// if x position exceeds 16th block then reset it and increment z position
		if(xPos >= 16) {
			zPos++;
			xPos = 0;
		}

		// if z position exceeds 16th block then reset it (as well as x) and increment y position
		if(zPos >= 16) {
			yPos++;
			zPos = 0;
			xPos = 0;
		}

	}


	// ---


	// reset coordinates values aftre that first loop
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

		// iterate through all blocks again
		for(int b = 0; b < blockAmount; b++) {
			// if both indexed blocks arent the same block and second block isnt just an air block
			if(i != b && newChunk.blockPositions[b][3] != 0) {
				
				// create and write new block position vectors for both blocks
				vec4 block1Pos;
				vec4 block2Pos;
				glm_vec4_copy(newChunk.blockPositions[i], block1Pos);
				glm_vec4_copy(newChunk.blockPositions[b], block2Pos);

				// front
				if(block1Pos[0] == block2Pos[0] && block1Pos[1] == block2Pos[1] && block1Pos[2]+1 == block2Pos[2]) {
					front = false;
				}
				// back
				if(block1Pos[0] == block2Pos[0] && block1Pos[1] == block2Pos[1] && block1Pos[2]-1 == block2Pos[2]) {
					back = false;
				}
				// left
				if(block1Pos[0]-1 == block2Pos[0] && block1Pos[1] == block2Pos[1] && block1Pos[2] == block2Pos[2]) {
					left = false;
				}
				// right
				if(block1Pos[0]+1 == block2Pos[0] && block1Pos[1] == block2Pos[1] && block1Pos[2] == block2Pos[2]) {
					right = false;
				}
				// bottom
				if(block1Pos[0] == block2Pos[0] && block1Pos[1]-1 == block2Pos[1] && block1Pos[2] == block2Pos[2]) {
					bottom = false;
				}
				// top
				if(block1Pos[0] == block2Pos[0] && block1Pos[1]+1 == block2Pos[1] && block1Pos[2] == block2Pos[2]) {
					top = false;
				}

			}
		}

		// set values to 0 based on side booleans
		

		if(front) {
			// generate proper vertices array and load it into frontVertices
			create_side_vertices("front", "grass", xPos, yPos, zPos, frontVertices);
			
			// generate proper indices array and load it into sideIndices
			create_side_indices(indicesOffset, i, sideIndices);

			// load proper vertices and indices array into VBO via glBufferSubData
			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + verticesIndex, sizeof(frontVertices), frontVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + indicesIndex, sizeof(sideIndices), sideIndices);

			// increment vertices and indices index, as well as indices offset
			verticesIndex += 4*8 * sizeof(float);
			indicesIndex += 6 * sizeof(int);
			indicesOffset += 4;
		}
		if(back) {
			// generate proper vertices array and load it into backVertices
			create_side_vertices("back", "grass", xPos, yPos, zPos, backVertices);

			// generate proper indices array and load it into sideIndices
			create_side_indices(indicesOffset, i, sideIndices);

			// load proper vertices and indices array into VBO via glBufferSubData
			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + verticesIndex, sizeof(backVertices), backVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + indicesIndex, sizeof(sideIndices), sideIndices);

			// increment vertices and indices index, as well as indices offset
			verticesIndex += 4*8 * sizeof(float);
			indicesIndex += 6 * sizeof(int);
			indicesOffset += 4;
		}
		if(left) {
			// generate proper vertices array and load it into leftVertices
			create_side_vertices("left", "grass",  xPos, yPos, zPos, leftVertices);

			// generate proper indices array and load it into sideIndices
			create_side_indices(indicesOffset, i, sideIndices);

			// load proper vertices and indices array into VBO via glBufferSubData
			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + verticesIndex, sizeof(leftVertices), leftVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + indicesIndex, sizeof(sideIndices), sideIndices);

			// increment vertices and indices index, as well as indices offset
			verticesIndex += 4*8 * sizeof(float);
			indicesIndex += 6 * sizeof(int);
			indicesOffset += 4;
		}
		if(right) {
			// generate proper vertices array and load it into rightVertices
			create_side_vertices("right", "grass", xPos, yPos, zPos, rightVertices);

			// generate proper indices array and load it into sideIndices
			create_side_indices(indicesOffset, i, sideIndices);

			// load proper vertices and indices array into VBO via glBufferSubData
			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + verticesIndex, sizeof(rightVertices), rightVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + indicesIndex, sizeof(sideIndices), sideIndices);

			// increment vertices and indices index, as well as indices offset
			verticesIndex += 4*8 * sizeof(float);
			indicesIndex += 6 * sizeof(int);
			indicesOffset += 4;
		}
		if(bottom) {
			// generate proper vertices array and load it into bottomVertices
			create_side_vertices("bottom", "grass", xPos, yPos, zPos, bottomVertices);

			// generate proper indices array and load it into sideIndices
			create_side_indices(indicesOffset, i, sideIndices);

			// load proper vertices and indices array into VBO via glBufferSubData
			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + verticesIndex, sizeof(bottomVertices), bottomVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + indicesIndex, sizeof(sideIndices), sideIndices);

			// increment vertices and indices index, as well as indices offset
			verticesIndex += 4*8 * sizeof(float);
			indicesIndex += 6 * sizeof(int);
			indicesOffset += 4;
		}
		if(top) {
			// generate proper vertices array and load it into topVertices
			create_side_vertices("top", "grass", xPos, yPos, zPos, topVertices);

			// generate proper indices array and load it into sideIndices
			create_side_indices(indicesOffset, i, sideIndices);

			// load proper vertices and indices array into VBO via glBufferSubData
			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + verticesIndex, sizeof(topVertices), topVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + indicesIndex, sizeof(sideIndices), sideIndices);
		}


		// load vertices data into vbo
		//glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices) * i, sizeof(vertices), vertices);

		// load indices data into ebo
		//glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * i, sizeof(indices), indices);

		// increment block count
		blockCount++;


		// ---


		xPos++;
	
		// if x position exceeds 16th block then reset it and increment z position
		if(xPos >= 16) {
			zPos++;
			xPos = 0;
		}

		// if z position exceeds 16th block then reset it (as well as x) and increment y position
		if(zPos >= 16) {
			yPos++;
			zPos = 0;
			xPos = 0;
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

	// apply block count to chunk
	newChunk.blockCount = blockCount;

	// return newly generated chunk object
	return newChunk;

}


// ---


void draw_chunk(struct Chunk chunk, unsigned int shaderProgram, unsigned int worldAtlas) {

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

	// get locations of uniform camera matrices
	int modelLoc = glGetUniformLocation(shaderProgram, "model");
	int viewLoc = glGetUniformLocation(shaderProgram, "view");
	int projLoc = glGetUniformLocation(shaderProgram, "proj");

	// load data into uniforms
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, *model);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *view);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, *proj);

	// draw the elements
	glDrawElements(GL_TRIANGLES, 36 * chunk.blockCount, GL_UNSIGNED_INT, 0);

}



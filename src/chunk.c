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

	xPos = 0;
	yPos = 0;
	zPos = 0;
	

	// now iterate (again) thru all block positions
	for(int i=0; i < blockAmount; i++) {

		// add on vertices index
		int verticesIndex = 0;

		int indicesIndex = 0;
		int indicesOffset = 0;

		// front vertices
		float frontVertices[] = {
			// position                           color                texture coords

			// front
			0.0f+xPos,  1.0f+yPos,  1.0f+zPos,    1.0f, 0.0f, 0.0f,    calc_at_tex_x(0),  calc_at_tex_y(0),   // top left
			1.0f+xPos,  1.0f+yPos,  1.0f+zPos,    0.0f, 1.0f, 0.0f,    calc_at_tex_x(16), calc_at_tex_y(0),   // top right
			0.0f+xPos,  0.0f+yPos,  1.0f+zPos,    0.0f, 0.0f, 1.0f,    calc_at_tex_x(0),  calc_at_tex_y(16),  // bot left
			1.0f+xPos,  0.0f+yPos,  1.0f+zPos,    1.0f, 0.0f, 1.0f,    calc_at_tex_x(16), calc_at_tex_y(16),  // bot right
		};
		float backVertices[] = {
			// position                           color                texture coords

			// back (flip x tex coords here so that back shows same image as rest and not flipped)
		   0.0f+xPos,  1.0f+yPos,  0.0f+zPos,    1.0f, 0.0f, 0.0f,    calc_at_tex_x(16), calc_at_tex_y(0),   // top left
			1.0f+xPos,  1.0f+yPos,  0.0f+zPos,    0.0f, 1.0f, 0.0f,    calc_at_tex_x(0),  calc_at_tex_y(0),   // top right
		   0.0f+xPos,  0.0f+yPos,  0.0f+zPos,    0.0f, 0.0f, 1.0f,    calc_at_tex_x(16), calc_at_tex_y(16),  // bot left
			1.0f+xPos,  0.0f+yPos,  0.0f+zPos,    1.0f, 0.0f, 1.0f,    calc_at_tex_x(0),  calc_at_tex_y(16),  // bot right			
		};
		float leftVertices[] = {
			// position                           color                texture coords

			// left
		   0.0f+xPos,  1.0f+yPos,  0.0f+zPos,    1.0f, 0.0f, 0.0f,    calc_at_tex_x(0),  calc_at_tex_y(0),   // top left
		   0.0f+xPos,  1.0f+yPos,  1.0f+zPos,    0.0f, 1.0f, 0.0f,    calc_at_tex_x(16), calc_at_tex_y(0),   // top right
		   0.0f+xPos,  0.0f+yPos,  0.0f+zPos,    0.0f, 0.0f, 1.0f,    calc_at_tex_x(0),  calc_at_tex_y(16),  // bot left
		   0.0f+xPos,  0.0f+yPos,  1.0f+zPos,    1.0f, 0.0f, 1.0f,    calc_at_tex_x(16), calc_at_tex_y(16),  // bot right
		};
		float rightVertices[] = {
			// position                           color                texture coords

			// right
			1.0f+xPos,  1.0f+yPos,  1.0f+zPos,    1.0f, 0.0f, 0.0f,    calc_at_tex_x(0),  calc_at_tex_y(0),   // top left
			1.0f+xPos,  1.0f+yPos,  0.0f+zPos,    0.0f, 1.0f, 0.0f,    calc_at_tex_x(16), calc_at_tex_y(0),   // top right
			1.0f+xPos,  0.0f+yPos,  1.0f+zPos,    0.0f, 0.0f, 1.0f,    calc_at_tex_x(0),  calc_at_tex_y(16),  // bot left
			1.0f+xPos,  0.0f+yPos,  0.0f+zPos,    1.0f, 0.0f, 1.0f,    calc_at_tex_x(16), calc_at_tex_y(16),  // bot right
		};
		float bottomVertices[] = {
			// position                           color                texture coords

			// bottom
		   0.0f+xPos,  0.0f+yPos,  1.0f+zPos,    1.0f, 0.0f, 0.0f,    calc_at_tex_x(16), calc_at_tex_y(0),   // top left
			1.0f+xPos,  0.0f+yPos,  1.0f+zPos,    0.0f, 1.0f, 0.0f,    calc_at_tex_x(32), calc_at_tex_y(0),   // top right
		   0.0f+xPos,  0.0f+yPos,  0.0f+zPos,    0.0f, 0.0f, 1.0f,    calc_at_tex_x(16), calc_at_tex_y(16),  // bot left
			1.0f+xPos,  0.0f+yPos,  0.0f+zPos,    1.0f, 0.0f, 1.0f,    calc_at_tex_x(32), calc_at_tex_y(16),  // bot right	
		};
		float topVertices[] = {
			// position                           color                texture coords

			// top
		   0.0f+xPos,  1.0f+yPos,  1.0f+zPos,    1.0f, 0.0f, 0.0f,    calc_at_tex_x(32), calc_at_tex_y(0),   // top left
			1.0f+xPos,  1.0f+yPos,  1.0f+zPos,    0.0f, 1.0f, 0.0f,    calc_at_tex_x(48), calc_at_tex_y(0),   // top right
		   0.0f+xPos,  1.0f+yPos,  0.0f+zPos,    0.0f, 0.0f, 1.0f,    calc_at_tex_x(32), calc_at_tex_y(16),  // bot left
			1.0f+xPos,  1.0f+yPos,  0.0f+zPos,    1.0f, 0.0f, 1.0f,    calc_at_tex_x(48), calc_at_tex_y(16),  // bot right
		};



		// indices array
		int indices[] = {
			// front
			0+(i*24), 1+(i*24), 2+(i*24),
			1+(i*24), 2+(i*24), 3+(i*24),

			// back
			4+(i*24), 5+(i*24), 6+(i*24),
			5+(i*24), 6+(i*24), 7+(i*24),

			// left
			8+(i*24), 9+(i*24), 10+(i*24),
			9+(i*24), 10+(i*24), 11+(i*24),

			// right
			12+(i*24), 13+(i*24), 14+(i*24),
			13+(i*24), 14+(i*24), 15+(i*24),

			// bottom
			16+(i*24), 17+(i*24), 18+(i*24),
			17+(i*24), 18+(i*24), 19+(i*24),

			// top
			20+(i*24), 21+(i*24), 22+(i*24),
			21+(i*24), 22+(i*24), 23+(i*24),
		};

		// booleans to tell whether to add those sides or not
		bool front = true;
		bool back = true;
		bool left = true;
		bool right = true;
		bool bottom = true;
		bool top = true;

		// iterate through all blocks again
		for(int b = 0; b < blockAmount; b++) {
			// if both indexed blocks arent the same block
			if(i != b) {
				
				// create and write new block position vectors for both blocks
				vec4 block1Pos;
				vec4 block2Pos;
				glm_vec4_copy(newChunk.blockPositions[i], block1Pos);
				glm_vec4_copy(newChunk.blockPositions[b], block2Pos);

				// if second block isnt just an air block
				if(block2Pos[3] != 0) {

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
		}

		// set values to 0 based on side booleans
		

		if(front) {

			int indices_addition[] = {
				indicesOffset+(i*24), indicesOffset+1+(i*24), indicesOffset+2+(i*24),
				indicesOffset+1+(i*24), indicesOffset+2+(i*24), indicesOffset+3+(i*24),
			};

			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + verticesIndex, sizeof(frontVertices), frontVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + indicesIndex, sizeof(indices_addition), indices_addition);

			verticesIndex += 4*8 * sizeof(float);
			indicesIndex += 6 * sizeof(int);
			indicesOffset += 4;
		}
		if(back) {

			int indices_addition[] = {
				indicesOffset+(i*24), indicesOffset+1+(i*24), indicesOffset+2+(i*24),
				indicesOffset+1+(i*24), indicesOffset+2+(i*24), indicesOffset+3+(i*24),
			};

			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + verticesIndex, sizeof(backVertices), backVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + indicesIndex, sizeof(indices_addition), indices_addition);

			verticesIndex += 4*8 * sizeof(float);
			indicesIndex += 6 * sizeof(int);
			indicesOffset += 4;
		}
		if(left) {

			int indices_addition[] = {
				indicesOffset+(i*24), indicesOffset+1+(i*24), indicesOffset+2+(i*24),
				indicesOffset+1+(i*24), indicesOffset+2+(i*24), indicesOffset+3+(i*24),
			};

			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + verticesIndex, sizeof(leftVertices), leftVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + indicesIndex, sizeof(indices_addition), indices_addition);

			verticesIndex += 4*8 * sizeof(float);
			indicesIndex += 6 * sizeof(int);
			indicesOffset += 4;
		}
		if(right) {

			int indices_addition[] = {
				indicesOffset+(i*24), indicesOffset+1+(i*24), indicesOffset+2+(i*24),
				indicesOffset+1+(i*24), indicesOffset+2+(i*24), indicesOffset+3+(i*24),
			};

			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + verticesIndex, sizeof(rightVertices), rightVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + indicesIndex, sizeof(indices_addition), indices_addition);

			verticesIndex += 4*8 * sizeof(float);
			indicesIndex += 6 * sizeof(int);
			indicesOffset += 4;
		}
		if(bottom) {

			int indices_addition[] = {
				indicesOffset+(i*24), indicesOffset+1+(i*24), indicesOffset+2+(i*24),
				indicesOffset+1+(i*24), indicesOffset+2+(i*24), indicesOffset+3+(i*24),
			};

			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + verticesIndex, sizeof(bottomVertices), bottomVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + indicesIndex, sizeof(indices_addition), indices_addition);

			verticesIndex += 4*8 * sizeof(float);
			indicesIndex += 6 * sizeof(int);
			indicesOffset += 4;
		}
		if(top) {

			int indices_addition[] = {
				indicesOffset+(i*24), indicesOffset+1+(i*24), indicesOffset+2+(i*24),
				indicesOffset+1+(i*24), indicesOffset+2+(i*24), indicesOffset+3+(i*24),
			};

			glBufferSubData(GL_ARRAY_BUFFER, (sizeof(float) * 6*4*8) * i + verticesIndex, sizeof(topVertices), topVertices);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(int) * 6*6) * i + indicesIndex, sizeof(indices_addition), indices_addition);
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



#include "../include/GLAD33/glad.h"
#include "../include/GLFW/glfw3.h"
#include <CGLM/cglm.h>

#include "headers/image.h"
#include "headers/camera.h"
#include "headers/mesh.h"


// ---


// block mesh
struct Mesh blockMesh;


// ---


// initiates and generates mesh for a single block
void init_test_block() {
	
	// vertices array
	float vertices[] = {
		// position           color                texture coords

		// front
	   0.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.0f,    calc_at_tex_x(0),  calc_at_tex_y(0),   // top left
		1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 0.0f,    calc_at_tex_x(16), calc_at_tex_y(0),   // top right
	   0.0f,  0.0f,  1.0f,    0.0f, 0.0f, 1.0f,    calc_at_tex_x(0),  calc_at_tex_y(16),  // bot left
		1.0f,  0.0f,  1.0f,    1.0f, 0.0f, 1.0f,    calc_at_tex_x(16), calc_at_tex_y(16),  // bot right

		// back (flip x tex coords here so that back shows same image as rest and not flipped)
	   0.0f,  1.0f,  0.0f,    1.0f, 0.0f, 0.0f,    calc_at_tex_x(16), calc_at_tex_y(0),   // top left
		1.0f,  1.0f,  0.0f,    0.0f, 1.0f, 0.0f,    calc_at_tex_x(0),  calc_at_tex_y(0),   // top right
	   0.0f,  0.0f,  0.0f,    0.0f, 0.0f, 1.0f,    calc_at_tex_x(16), calc_at_tex_y(16),  // bot left
		1.0f,  0.0f,  0.0f,    1.0f, 0.0f, 1.0f,    calc_at_tex_x(0),  calc_at_tex_y(16),  // bot right
		
		// left
	   0.0f,  1.0f,  0.0f,    1.0f, 0.0f, 0.0f,    calc_at_tex_x(0),  calc_at_tex_y(0),   // top left
	   0.0f,  1.0f,  1.0f,    0.0f, 1.0f, 0.0f,    calc_at_tex_x(16), calc_at_tex_y(0),   // top right
	   0.0f,  0.0f,  0.0f,    0.0f, 0.0f, 1.0f,    calc_at_tex_x(0),  calc_at_tex_y(16),  // bot left
	   0.0f,  0.0f,  1.0f,    1.0f, 0.0f, 1.0f,    calc_at_tex_x(16), calc_at_tex_y(16),  // bot right

		// right
	   1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.0f,    calc_at_tex_x(0),  calc_at_tex_y(0),   // top left
	   1.0f,  1.0f,  0.0f,    0.0f, 1.0f, 0.0f,    calc_at_tex_x(16), calc_at_tex_y(0),   // top right
	   1.0f,  0.0f,  1.0f,    0.0f, 0.0f, 1.0f,    calc_at_tex_x(0),  calc_at_tex_y(16),  // bot left
	   1.0f,  0.0f,  0.0f,    1.0f, 0.0f, 1.0f,    calc_at_tex_x(16), calc_at_tex_y(16),  // bot right

		// bottom
	   0.0f,  0.0f,  1.0f,    1.0f, 0.0f, 0.0f,    calc_at_tex_x(16), calc_at_tex_y(0),   // top left
	   1.0f,  0.0f,  1.0f,    0.0f, 1.0f, 0.0f,    calc_at_tex_x(32), calc_at_tex_y(0),   // top right
	   0.0f,  0.0f,  0.0f,    0.0f, 0.0f, 1.0f,    calc_at_tex_x(16), calc_at_tex_y(16),  // bot left
	   1.0f,  0.0f,  0.0f,    1.0f, 0.0f, 1.0f,    calc_at_tex_x(32), calc_at_tex_y(16),  // bot right
	
		// top
	   0.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.0f,    calc_at_tex_x(32), calc_at_tex_y(0),   // top left
	   1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 0.0f,    calc_at_tex_x(48), calc_at_tex_y(0),   // top right
	   0.0f,  1.0f,  0.0f,    0.0f, 0.0f, 1.0f,    calc_at_tex_x(32), calc_at_tex_y(16),  // bot left
	   1.0f,  1.0f,  0.0f,    1.0f, 0.0f, 1.0f,    calc_at_tex_x(48), calc_at_tex_y(16),  // bot right
	};
	// indices array
	int indices[] = {
		// front
		0, 1, 2,
		1, 2, 3,

		// back
		4, 5, 6,
		5, 6, 7,

		// left
		8, 9, 10,
		9, 10, 11,

		// right
		12, 13, 14,
		13, 14, 15,

		// bottom
		16, 17, 18,
		17, 18, 19,

		// top
		20, 21, 22,
		21, 22, 23,
	};


	// ---
	

	// create vbo
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	// bind vbo
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// load vertices into vbo
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// unbind vbo for now
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// ---
	

	// create ebo
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	// bind ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	// load indices into ebo
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// unbind ebo for now
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
	

	// assign vao, vbo, and ebo to mesh
	blockMesh.vao = VAO;
	blockMesh.vbo = VBO;
	blockMesh.ebo = EBO;
}


// ---


void draw_test_block(unsigned int shaderProgram, unsigned int worldAtlas) {

	// bind vao
	glBindVertexArray(blockMesh.vao);

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
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}


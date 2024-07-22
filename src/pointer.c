#include <GLAD33/glad.h>
#include <GLFW/glfw3.h>
#include <CGLM/cglm.h>

#include "headers/camera.h"
#include "headers/image.h"
#include "headers/mesh.h"
#include "headers/shader.h"


// ---


// select block mesh
struct Mesh selectedBlockMesh;

// crosshair mesh
struct Mesh crosshairMesh;

// select block shader program
unsigned int selectShaderProgram;

// crosshair shader program
unsigned int crosshairShaderProgram;

// how many blocks ahead can the pointer reach
const int POINTER_REACH = 6;


// ---


// initiates and generates mesh for a single block
void init_select_block() {
	
	// vertices array
	float vertices[] = {
		// position               color                texture coords

		// front
	  -0.01f,  1.02f,  1.02f,    1.0f, 1.0f, 1.0f,    calc_at_tex_x(0),  calc_at_tex_y(32),   // top left
		1.02f,  1.02f,  1.02f,    1.0f, 1.0f, 1.0f,    calc_at_tex_x(16), calc_at_tex_y(32),   // top right
	  -0.01f, -0.01f,  1.02f,    1.0f, 1.0f, 1.0f,    calc_at_tex_x(0),  calc_at_tex_y(48),   // bot left
		1.02f, -0.01f,  1.02f,    1.0f, 1.0f, 1.0f,    calc_at_tex_x(16), calc_at_tex_y(48),   // bot right

		// right
	   1.02f,  1.02f,  1.02f,    1.0f, 1.0f, 1.0f,    calc_at_tex_x(0),  calc_at_tex_y(32),   // top left
	   1.02f,  1.02f, -0.01f,    1.0f, 1.0f, 1.0f,    calc_at_tex_x(16), calc_at_tex_y(32),   // top right
	   1.02f, -0.01f,  1.02f,    1.0f, 1.0f, 1.0f,    calc_at_tex_x(0),  calc_at_tex_y(48),   // bot left
	   1.02f, -0.01f, -0.01f,    1.0f, 1.0f, 1.0f,    calc_at_tex_x(16), calc_at_tex_y(48),   // bot right
	
		// top
	  -0.01f,  1.02f,  1.02f,    1.0f, 1.0f, 1.0f,    calc_at_tex_x(0),  calc_at_tex_y(32),   // top left
	   1.02f,  1.02f,  1.02f,    1.0f, 1.0f, 1.0f,    calc_at_tex_x(16), calc_at_tex_y(32),   // top right
	  -0.01f,  1.02f, -0.01f,    1.0f, 1.0f, 1.0f,    calc_at_tex_x(0),  calc_at_tex_y(48),   // bot left
	   1.02f,  1.02f, -0.01f,    1.0f, 1.0f, 1.0f,    calc_at_tex_x(16), calc_at_tex_y(48),   // bot right
	};
	// indices array
	int indices[] = {
		// front
		0, 1, 2,
		1, 2, 3,

		// right
		4, 5, 6,
		5, 6, 7,

		// top
		8, 9, 10,
		9, 10, 11,
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
	

	// create shader program
	selectShaderProgram = create_shader_program("shaders/select_shader.vert", "shaders/select_shader.frag");
	

	// assign vao, vbo, and ebo to mesh
	selectedBlockMesh.vao = VAO;
	selectedBlockMesh.vbo = VBO;
	selectedBlockMesh.ebo = EBO;
}


// ---


void draw_select_block(unsigned int worldAtlas) {



	// bind vao
	glBindVertexArray(selectedBlockMesh.vao);

	// use shader program
	glUseProgram(selectShaderProgram);

	// bind texture
	glBindTexture(GL_TEXTURE_2D, worldAtlas);

	// get camera matrices
	mat4* model = get_model();
	mat4* view = get_view();
	mat4* proj = get_projection();

	// get camera front
	vec3* camFront = get_camera_front();


	// get locations of uniform camera matrices
	int modelLoc = glGetUniformLocation(selectShaderProgram, "model");
	int viewLoc = glGetUniformLocation(selectShaderProgram, "view");
	int projLoc = glGetUniformLocation(selectShaderProgram, "proj");

	// get location of position uniform
	int posLoc = glGetUniformLocation(selectShaderProgram, "position");

	// load data into uniforms
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *view);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, *proj);


	// ---
	

	// get camera position
	vec3* camPos = get_camera_pos();

	// position for select block to render
	vec3 selectPos = GLM_VEC3_ZERO_INIT;

	selectPos[0] = round((*camPos)[0] + ( (*camFront)[0]*POINTER_REACH ));
	selectPos[1] = round((*camPos)[1] + ( (*camFront)[1]*POINTER_REACH ));
	selectPos[2] = round((*camPos)[2] + ( (*camFront)[2]*POINTER_REACH ));

	
	// ---
	

	mat4 newModel;
	glm_mat4_copy(*model, newModel);

	// x + z axis

	if((*camFront)[0] > 0 && (*camFront)[2] > 0) {

		// side
		glm_translate(newModel, (vec3){0, 0, -1.03f});
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, newModel);
		// draw the elements
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// clear newModel matrix
		glm_mat4_copy(*model, newModel);

		// other side
		glm_translate(newModel, (vec3){-1.03f, 0, 0});
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, newModel);
		// draw the elements
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(int)));

	}
	if((*camFront)[0] < 0 && (*camFront)[2] > 0) {

		// side
		glm_translate(newModel, (vec3){0, 0, -1.03f});
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, newModel);
		// draw the elements
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// clear newModel matrix
		glm_mat4_copy(*model, newModel);

		// other side
		glm_translate(newModel, (vec3){0, 0, 0});
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, newModel);
		// draw the elements
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(int)));

	}
	if((*camFront)[0] < 0 && (*camFront)[2] < 0) {

		// side
		glm_translate(newModel, (vec3){0, 0, 0});
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, newModel);
		// draw the elements
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// clear newModel matrix
		glm_mat4_copy(*model, newModel);

		// other side
		glm_translate(newModel, (vec3){0, 0, 0});
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, newModel);
		// draw the elements
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(int)));

	}
	if((*camFront)[0] > 0 && (*camFront)[2] < 0) {

		// side
		glm_translate(newModel, (vec3){0, 0, 0});
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, newModel);
		// draw the elements
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// clear newModel matrix
		glm_mat4_copy(*model, newModel);

		// other side
		glm_translate(newModel, (vec3){-1.03f, 0, 0});
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, newModel);
		// draw the elements
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(int)));

	}

	// y axis
	
	if((*camFront)[1] > 0) {
		// clear newModel matrix
		glm_mat4_copy(*model, newModel);

		// bottom
		glm_translate(newModel, (vec3){0, -1.03f, 0});
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, newModel);
		// draw the elements
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(int)));
	}
	else {
		// clear newModel matrix
		glm_mat4_copy(*model, newModel);

		// top
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, newModel);
		// draw the elements
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(int)));
	}


	// ---
	
	
	// load position uniform
	glUniform3f(posLoc, selectPos[0], selectPos[1], selectPos[2]);

}


// ---


void init_crosshair() {
	
	// vertices array
	float vertices[] = {
		// position               color                texture coords

		// front
	   -0.04f,  0.04f,  0.0f,    1.0f, 1.0f, 1.0f,    calc_at_tex_x(0),  calc_at_tex_y(48),   // top left
		0.04f,  0.04f,  0.0f,    1.0f, 1.0f, 1.0f,    calc_at_tex_x(4),  calc_at_tex_y(48),   // top right
	   -0.04f, -0.04f,  0.0f,    1.0f, 1.0f, 1.0f,     calc_at_tex_x(0),  calc_at_tex_y(52),   // bot left
		0.04f, -0.04f,  0.0f,    1.0f, 1.0f, 1.0f,     calc_at_tex_x(4),  calc_at_tex_y(52),   // bot right
	};
	// indices array
	int indices[] = {
		// front
		0, 1, 2,
		1, 2, 3,
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
	

	// create shader program
	crosshairShaderProgram = create_shader_program("shaders/crosshair_shader.vert", "shaders/crosshair_shader.frag");
	

	// assign vao, vbo, and ebo to mesh
	crosshairMesh.vao = VAO;
	crosshairMesh.vbo = VBO;
	crosshairMesh.ebo = EBO;
}


// ---


void draw_crosshair(unsigned int worldAtlas) {

	// bind vao
	glBindVertexArray(crosshairMesh.vao);

	// use shader program
	glUseProgram(crosshairShaderProgram);

	// bind texture
	glBindTexture(GL_TEXTURE_2D, worldAtlas);

	// draw the elements
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}


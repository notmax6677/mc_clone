#include <GLAD33/glad.h>
#include <GLFW/glfw3.h>
#include <CGLM/cglm.h>

#include <stdlib.h>

#include "headers/chunk.h"
#include "headers/world.h"
#include "headers/camera.h"
#include "headers/image.h"
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
const int POINTER_REACH = 30;

// divides the pointer incrementations by it, making the pointer more accurate,
// but it will also require a higher pointer reach, which may haev an effect on performance
const int POINTER_REDUCER = 3;

// select block position
vec3 selectPos = GLM_VEC3_ZERO_INIT;

// last select block position
vec3 lastSelectPos = GLM_VEC3_ZERO_INIT;

// whether or not currently selecting something
bool selectingSomething = false;

// type of currently selected block
int selectBlockType = 0;

// pointer to currently selected chunk
struct Chunk* selectChunk;


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


	// ---
	

	// also allocate space for selectChunk on heap
	selectChunk = malloc( sizeof(struct Chunk) );
}


// ---


void update_select_block() {
	// get camera position
	vec3* camPos = get_camera_pos();

	// get camera front
	vec3* camFront = get_camera_front();

	// adjust camera position to recenter for ray to be emitted from middle of window
	vec3 adjustedCamPos;
	glm_vec3_copy( (vec3){
		(*camPos)[0]-0.5,
		(*camPos)[1]-0.5,
		(*camPos)[2]-0.5
	}, adjustedCamPos );

	// iterations to move pointer along a line
	int iterations = 0;

	// the type of the block being looked at
	selectBlockType = 0;

	// position of chunk that camera is in
	vec2 playerChunkPos;

	// select position relative to chunk
	vec3 relativeSelectPos;

	// initiate by copying over current camera position to select position
	glm_vec3_copy(adjustedCamPos, selectPos);

	while(iterations < POINTER_REACH) {
		// get player chunk position
		playerChunkPos[0] = floor( round(selectPos[0]) / get_chunk_width() );
		playerChunkPos[1] = floor( round(selectPos[2]) / get_chunk_length() );

		// get the current chunk
		*selectChunk = get_chunk(playerChunkPos[0], playerChunkPos[1]);

		// copy over selectPos to relativeSelectPos
		glm_vec3_copy(selectPos, relativeSelectPos);

		// remove chunk position 
		relativeSelectPos[0] -= (*selectChunk).pos[0]*get_chunk_width();
		relativeSelectPos[2] -= (*selectChunk).pos[1]*get_chunk_length();

		// round the x y z values
		relativeSelectPos[0] = round(relativeSelectPos[0]);
		relativeSelectPos[1] = round(relativeSelectPos[1]);
		relativeSelectPos[2] = round(relativeSelectPos[2]);

		// if not out of y bounds (ABOVE OR BELOW CHUNK)
		if(selectPos[1] > get_chunk_height() || selectPos[1] < 0) {
			selectBlockType = 0;
		}
		else {
			// get the block type of the block at that position
			selectBlockType = get_block_type((*selectChunk).blockTypes, relativeSelectPos[0], relativeSelectPos[1], relativeSelectPos[2]);
		}

		// if block type is air, then move further along the path
		if(selectBlockType == 0) {
			// but before moving along path, copy to last select position
			glm_vec3_copy(selectPos, lastSelectPos);
			
			// move along path
			glm_vec3_add(selectPos, (vec3){
				(*camFront)[0]/POINTER_REDUCER,
				(*camFront)[1]/POINTER_REDUCER,
				(*camFront)[2]/POINTER_REDUCER
			}, selectPos);
		}

		// increment interations
		iterations++;
	}

	// if blocktype is still air by the end, then just move the select position to 0, 0, 0 to avoid it being shown
	if(selectBlockType == 0) {
		glm_vec3_copy(GLM_VEC3_ZERO, selectPos);
		// set selectingSomething to false
		selectingSomething = false;
	}
	else {
		// enable selectingSomething
		selectingSomething = true;
	}
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

	// load position uniform
	glUniform3f(posLoc, round(selectPos[0]), round(selectPos[1]), round(selectPos[2]));

	
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

}


// ---


void init_crosshair() {
	
	// vertices array
	float vertices[] = {
		// position               color                texture coords

		// front
	  -0.04f,  0.04f,  0.0f,    1.0f, 1.0f, 1.0f,     calc_at_tex_x(0),  calc_at_tex_y(48),   // top left
		0.04f,  0.04f,  0.0f,    1.0f, 1.0f, 1.0f,     calc_at_tex_x(4),  calc_at_tex_y(48),   // top right
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


// ---


void init_pointer() {
	init_select_block();
	init_crosshair();
}

void update_pointer(GLFWwindow* window) {
	update_select_block();
}

void draw_pointer(unsigned int worldAtlas) {
	draw_select_block(worldAtlas);
	draw_crosshair(worldAtlas);
}


// ---


void place_block() {

	// if even selecting something in the first place
	if(selectingSomething) {
		// vector to store player chunk position
		vec2 playerChunkPos = GLM_VEC2_ZERO_INIT;

		// get player chunk position
		playerChunkPos[0] = floor( round(lastSelectPos[0]) / get_chunk_width() );
		playerChunkPos[1] = floor( round(lastSelectPos[2]) / get_chunk_length() );


		// ---


		// vector for relative position
		vec3 relativeSelectPos;

		// copy over selectPos to relativeSelectPos
		glm_vec3_copy(lastSelectPos, relativeSelectPos);

		// remove chunk position 
		relativeSelectPos[0] -= (*selectChunk).pos[0]*get_chunk_width();
		relativeSelectPos[2] -= (*selectChunk).pos[1]*get_chunk_length();

		// round the x y z values
		relativeSelectPos[0] = round(relativeSelectPos[0]);
		relativeSelectPos[1] = round(relativeSelectPos[1]);
		relativeSelectPos[2] = round(relativeSelectPos[2]);


		// ---


		struct Chunk* newChunk;
		newChunk = malloc( sizeof(struct Chunk) );

		*newChunk = get_chunk(playerChunkPos[0], playerChunkPos[1]);
		int index = get_chunk_index(playerChunkPos[0], playerChunkPos[1]);

		*newChunk = insert_block(newChunk, (vec4){relativeSelectPos[0], relativeSelectPos[1], relativeSelectPos[2], 1});

		set_chunk(index, newChunk);


		free(newChunk);


	}

}

void delete_block() {

	// if even selecting something in the first place
	if(selectingSomething) {

		// vector to store player chunk position
		vec2 playerChunkPos = GLM_VEC2_ZERO_INIT;

		// get player chunk position
		playerChunkPos[0] = floor( round(selectPos[0]) / get_chunk_width() );
		playerChunkPos[1] = floor( round(selectPos[2]) / get_chunk_length() );


		// ---


		// vector for relative position
		vec3 relativeSelectPos;

		// copy over selectPos to relativeSelectPos
		glm_vec3_copy(selectPos, relativeSelectPos);

		// remove chunk position 
		relativeSelectPos[0] -= (*selectChunk).pos[0]*get_chunk_width();
		relativeSelectPos[2] -= (*selectChunk).pos[1]*get_chunk_length();

		// round the x y z values
		relativeSelectPos[0] = round(relativeSelectPos[0]);
		relativeSelectPos[1] = round(relativeSelectPos[1]);
		relativeSelectPos[2] = round(relativeSelectPos[2]);


		// ---


		struct Chunk* newChunk;
		newChunk = malloc( sizeof(struct Chunk) );

		*newChunk = get_chunk(playerChunkPos[0], playerChunkPos[1]);
		int index = get_chunk_index(playerChunkPos[0], playerChunkPos[1]);

		*newChunk = insert_block(newChunk, (vec4){relativeSelectPos[0], relativeSelectPos[1], relativeSelectPos[2], 0});

		set_chunk(index, newChunk);


		free(newChunk);


	}

}


// ---


// callback for mouse input, but dedicated to pointer related functionality
void pointer_mouse_input_callback(GLFWwindow* window, int button, int action, int mods) {
	// if left click
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		delete_block(); // delete block
	}
	// if right click
	if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		place_block(); // place block
	}
}

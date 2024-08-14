#include "../include/GLAD33/glad.h"
#include <GLFW/glfw3.h>
#include <CGLM/cglm.h>

#include "headers/image.h"
#include "headers/mesh.h"
#include "headers/shader.h"


// ---


// shader program for menu
unsigned int menuShader;

// width and height of menu bar
const float BAR_WIDTH = 1.0f;
const float BAR_HEIGHT = BAR_WIDTH * 0.1667f; // to keep accurate aspect ratio of menu bar

// width of select item (height isnt necessary because its the same as height of bar)
const float ITEM_WIDTH = BAR_WIDTH / 6.0f;

// mesh for menu bar
struct Mesh barMesh;

// mesh for selected item
struct Mesh itemMesh;

// menu bar offset on 2d plane of screen
vec2 menuOffset = (vec2){0.0f, -1.0f + BAR_HEIGHT};

// initial offset for item on x axis
const float initialItemXOffset = -BAR_WIDTH/2 + ITEM_WIDTH/4;

// added on to the x axis value of selected item based on what is selected
const float selectedItemOffset = ITEM_WIDTH * ((float)9/16);

// the actual item thats selected
int selectedItem = 1;


// ---


// getter for selectedItem
int get_selected_item() {
	return selectedItem;
}


// ---


// initiates the menu bar
void init_bar() {

	// vertices array
	float vertices[] = {
		// position                      texture coordinates

	  -BAR_WIDTH/2,  BAR_HEIGHT/2,      calc_at_tex_x(32),  calc_at_tex_y(48),   // top left
		BAR_WIDTH/2,  BAR_HEIGHT/2,      calc_at_tex_x(128), calc_at_tex_y(48),   // top right
	  -BAR_WIDTH/2, -BAR_HEIGHT/2,      calc_at_tex_x(32),  calc_at_tex_y(64),   // bottom left
	   BAR_WIDTH/2, -BAR_HEIGHT/2,      calc_at_tex_x(128), calc_at_tex_y(64),   // bottom right

	};

	// indices array
	int indices[] = {
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
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// unbind vao
	glBindVertexArray(0);

	// unbind vbo and ebo
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// ---


	// apply buffers and vertex array to bar mesh object
	barMesh.vao = VAO;
	barMesh.vbo = VBO;
	barMesh.ebo = EBO;

}

// draws the menu bar
void draw_bar(GLFWwindow* window, unsigned int worldAtlas) {

	// bind vao
	glBindVertexArray(barMesh.vao);

	// use shader program
	glUseProgram(menuShader);

	// bind texture
	glBindTexture(GL_TEXTURE_2D, worldAtlas);

	// get location of uniform vector2 for menu offset
	int offsetLoc = glGetUniformLocation(menuShader, "offset");

	// get location of world size vector2 uniform
	int windowSizeLoc = glGetUniformLocation(menuShader, "windowSize");


	// ---


	// pass offset vector as uniform to vertex shader
	glUniform2f(offsetLoc, menuOffset[0], menuOffset[1]);


	// ---
	

	// window width and height integers to later be filled
	int width, height;

	// get the window size and fill width/height values with data
	glfwGetWindowSize(window, &width, &height);

	// pass width and height as a uniform vector2 to the vertex shader
	glUniform2f(windowSizeLoc, width, height);


	// ---
	

	// draw the elements
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}


// ---


// initiates the menu bar
void init_item() {

	// vertices array
	float vertices[] = {
		// position                      texture coordinates

	   0.0f,        BAR_HEIGHT/2,       calc_at_tex_x(144), calc_at_tex_y(48),   // top left
		ITEM_WIDTH,  BAR_HEIGHT/2,       calc_at_tex_x(160), calc_at_tex_y(48),   // top right
	   0.0f,       -BAR_HEIGHT/2,       calc_at_tex_x(144), calc_at_tex_y(64),   // bottom left
	   ITEM_WIDTH, -BAR_HEIGHT/2,       calc_at_tex_x(160), calc_at_tex_y(64),   // bottom right

	};

	// indices array
	int indices[] = {
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
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// unbind vao
	glBindVertexArray(0);

	// unbind vbo and ebo
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// ---


	// apply buffers and vertex array to item mesh object
	itemMesh.vao = VAO;
	itemMesh.vbo = VBO;
	itemMesh.ebo = EBO;

}

// draws the menu bar
void draw_item(GLFWwindow* window, unsigned int worldAtlas) {

	// bind vao
	glBindVertexArray(itemMesh.vao);

	// use shader program
	glUseProgram(menuShader);

	// bind texture
	glBindTexture(GL_TEXTURE_2D, worldAtlas);

	// get location of uniform vector2 for menu offset
	int offsetLoc = glGetUniformLocation(menuShader, "offset");

	// get location of world size vector2 uniform
	int windowSizeLoc = glGetUniformLocation(menuShader, "windowSize");


	// ---


	// pass offset vector as uniform to vertex shader
	glUniform2f(offsetLoc, menuOffset[0] + initialItemXOffset + (selectedItem-1)*selectedItemOffset, menuOffset[1]);


	// ---
	

	// window width and height integers to later be filled
	int width, height;

	// get the window size and fill width/height values with data
	glfwGetWindowSize(window, &width, &height);

	// pass width and height as a uniform vector2 to the vertex shader
	glUniform2f(windowSizeLoc, width, height);


	// ---
	

	// draw the elements
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}


// ---


void init_menu() {
	// create menu shader object
	menuShader = create_shader_program("shaders/menu_shader.vert", "shaders/menu_shader.frag");

	init_bar();

	init_item();
}

void update_menu(GLFWwindow* window) {

	// set each number key to correspond to a type of block

	if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		selectedItem = 1;
	}
	else if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		selectedItem = 2;
	}
	else if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		selectedItem = 3;
	}
	else if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
		selectedItem = 4;
	}
	else if(glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
		selectedItem = 5;
	}
	else if(glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
		selectedItem = 6;
	}
	else if(glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
		selectedItem = 7;
	}
	else if(glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
		selectedItem = 8;
	}
	else if(glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
		selectedItem = 9;
	}
	else if(glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
		selectedItem = 10;
	}

}

void draw_menu(GLFWwindow* window, unsigned int worldAtlas) {
	// temporarily disable depth testing to draw this bit here
	glDisable(GL_DEPTH_TEST);

	draw_bar(window, worldAtlas);

	draw_item(window, worldAtlas);

	// re-enable depth testing to draw everything else
	glEnable(GL_DEPTH_TEST);
}

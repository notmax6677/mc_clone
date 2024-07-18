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


// ---


// world texture atlas
unsigned int worldAtlas;
// shader program for blocks
unsigned int blockShaderProgram;


// ---


struct Chunk firstChunk;


// ---


void init_world() {
	// create shaderprogram
	blockShaderProgram = create_shader_program("shaders/block_shader.vert", "shaders/block_shader.frag");

	// create texture atlas object
	worldAtlas = load_texture("assets/atlas.png");

	init_test_block();

	firstChunk = generate_chunk();
}

void draw_world() {
	draw_chunk(firstChunk, blockShaderProgram, worldAtlas);

	//draw_test_block(blockShaderProgram, worldAtlas);
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

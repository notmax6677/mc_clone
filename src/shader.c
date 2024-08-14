#include "../include/GLAD33/glad.h"
#include "../include/GLFW/glfw3.h"
#include <CGLM/cglm.h>

#include "headers/file.h"

// creates a shader program and returns it
unsigned int create_shader_program(char* vertexPath, char* fragmentPath) {

	// variables for testing itegrity of shader throughout its creation
	int success;
	char infoLog[512];


	// ---
	
	
	// create vertex shader object
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// load vertex shader source
	char* vertexShaderSource = load_file(vertexPath);
	// load vertex shader source into vertex shader
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

	// compile vertex shader
	glCompileShader(vertexShader);

	// run test for vertex shader
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	// if not successful
	if(!success) {
		// get info log and print it in error message
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("ERROR: Failed to compile vertex shader: \n%s", infoLog);
		return 0;
	}

	
	// ---
	

	// create fragment shader object
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// load fragment shader source
	char* fragmentShaderSource = load_file(fragmentPath);
	// load fragment shader source into fragment shader
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

	// compile fragment shader
	glCompileShader(fragmentShader);

	// run test for fragment shader
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	// if not successful
	if(!success) {
		// get info log and print it in error message
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("ERROR: Failed to compile fragment shader: \n%s", infoLog);
		return 0;
	}


	// ---
	

	// create shader program
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	// attach shaders to program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// link shader program
	glLinkProgram(shaderProgram);

	// run test for shader program
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	// if not successful
	if(!success) {
		// get info log and print it in error message
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("ERROR: Failed to link shader program: \n%s", infoLog);
		return 0;
	}


	// ---
	

	// cleanup
	
	// delete shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// free shader sources from heap
	free(vertexShaderSource);
	free(fragmentShaderSource);


	// ---
	

	// return shader program
	return shaderProgram;
}

#define STB_IMAGE_IMPLEMENTATION
#include "../include/STB/stb_image.h"

#include "../include/GLAD33/glad.h"
#include "../include/GLFW/glfw3.h"
#include "../include/CGLM/cglm.h"

// loads an image texture and returns the gl reference object to it
unsigned int load_texture(char* path) {

	// enable vertically flipping image
	stbi_set_flip_vertically_on_load(true);

	// declare width, height, and number of color channels variables
	int width, height, nrChannels;

	// use stb to load the data
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

	
	// ---
	
	
	// generate texture object
	unsigned int texture;
	glGenTextures(1, &texture);

	// bind texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// set clamp to edge
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// nearest + nearest texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// if data was properly loaded
	if(data) {
		// actually load the image data onto the texture object
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		// generate mip maps
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		// otherwise print error message
		printf("ERROR: Failed to load texture.");
	}


	// ---
	

	// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// free image from memory
	stbi_image_free(data);

	// return the texture
	return texture;
}

// calculate atlas texture x coordinate by inputting the atlas image's x coordinate
// assumes atlas size is 256x256
float calc_at_tex_x(float x) {
	return x/256;
}
// calculate atlas texture x coordinate by inputting the atlas image's x coordinate
// assumes atlas size is 256x256
float calc_at_tex_y(float y) {
	return 1.0f - (y/256);
}

#include <GLAD33/glad.h>
#include <GLFW/glfw3.h>
#include <CGLM/cglm.h>

#include "headers/shader.h"
#include "headers/image.h"
#include "headers/chunk.h"

#include "headers/camera.h"


// shader program for sun
unsigned int sunShaderProgram;

// mesh for sun object
struct Mesh sunMesh;

// sky color in R G B values
vec3 skyColor = GLM_VEC3_ZERO_INIT;

// the actual sky color that is returned in get_sky_col, this applies brightness based on time and day and stuff
vec3 returnedSkyColor = GLM_VEC3_ZERO_INIT;


// ---


// some settings

// distance of the sun, the higher the number, the smaller the sun will appear
const float SUN_DISTANCE = 5.0f;

// time speed
const float timeSpeed = 2.0f;
const float underWorldTimeSpeed = timeSpeed * 2.5f; // time speed whilst the sun is under the world

// hours
const float MAX_HOURS = 24.0f;

// time
float dayTime = MAX_HOURS * 0.75f; // offset by 0.5f because thats the half width of sun

// speed at which the sky color changes
const float COLOR_CHANGE_SPEED = 4.0f;

// its the day
bool isDay = false;
// reached half the day
bool halfDay = false;

// day color
vec3 dayColor = (vec3){163, 205, 227};
// night color
vec3 nightColor = (vec3){32, 26, 59};

// sun color
vec3 sunColor = (vec3){247, 207, 30};
// moon color
vec3 moonColor = (vec3){255, 255, 255};

// minimum color
vec3 minSkyColor = (vec3){14, 18, 43};

// changed from day to night or from night to day
bool changedDayMode = false;


// ---


// shade that is applied to all blocks
float blockShading = 1.0f;

float targetBlockShading = 1.0f;

// block shading that is applied during day
const float dayBlockShading = 1.0f;

// block shading that is applied during night
const float nightBlockShading = 0.4f;

// minimum block shading value
const float minimumBlockShading = 0.5f;


// ---


// returns the current block shading
float get_block_shading() {
	return blockShading;
}


// ---


// getter for sky color
vec3* get_sky_col() {
	return &returnedSkyColor;
}


// ---


// initiates sky related processes
void init_sky() {

	// create vertices array
	float vertices[] = {
		// position             texture coords
	  -0.5f,  0.5f,  0.0f,     calc_at_tex_x(192), calc_at_tex_y(16),     // top left
		0.5f,  0.5f,  0.0f,     calc_at_tex_x(240), calc_at_tex_y(16),     // top right
	  -0.5f, -0.5f,  0.0f,     calc_at_tex_x(192), calc_at_tex_y(64),     // bottom left
		0.5f, -0.5f,  0.0f,     calc_at_tex_x(240), calc_at_tex_y(64),     // bottom right
	};

	// create indices array
	int indices[] = {
		0, 1, 2,
		1, 2, 3
	};

	
	// ---
	

	// create vbo
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	// bind vbo
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// load data into vbo
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// unbind vbo for now
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// ---
	

	// create ebo
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	// bind ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	// load data into ebo
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture coords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// unbind vao
	glBindVertexArray(0);

	// unbind vbo and ebo
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	
	// ---
	

	// set mesh values to save the buffers and vao
	sunMesh.vao = VAO;
	sunMesh.vbo = VBO;
	sunMesh.ebo = EBO;

	// also load shader program
	sunShaderProgram = create_shader_program("shaders/sun_shader.vert", "shaders/sun_shader.frag");

}


// ---


// updates some sky related processes
void update_sky(float deltaTime) {

	// if dayTime is in given range (around under the world) 60-90%
	if(dayTime > MAX_HOURS * 0.6f && dayTime < MAX_HOURS * 0.9f) {
		// increment the day time with faster time speed
		dayTime += underWorldTimeSpeed * deltaTime;
	}
	else { // otherise (normal)
		// increment the day time
		dayTime += timeSpeed * deltaTime;
 	}

	// if day time has exceeded maximum amount in a day, reset it to 0
	if(dayTime >= MAX_HOURS) {
		// reset dayTime count
		dayTime = 0;

		// reset changedDayMode to false
		changedDayMode = false;
	}

	// if day is past 75% (so right about below earth) and the day mode hasn't been switched during that cycle
	if(dayTime >= MAX_HOURS * 0.75f && !changedDayMode) {
	 	// invert isDay
		isDay = !isDay;

		// if its day
		if(isDay) {
			glm_vec3_copy(dayColor, skyColor); // set sky color equal to day color values
		}
		else {
			glm_vec3_copy(nightColor, skyColor); // set sky color equal to night color values
		}

		// enable changedDayMode
		changedDayMode = true;
	}

	// if day is more than half
	if(dayTime >= MAX_HOURS/2) {
		halfDay = true;
	}
	else {
		halfDay = false;
	}

	// below world
	if(halfDay) {
		returnedSkyColor[0] = skyColor[0] * ( 1 - ( (dayTime - MAX_HOURS/2) / (MAX_HOURS/2) * COLOR_CHANGE_SPEED ) );
		returnedSkyColor[1] = skyColor[1] * ( 1 - ( (dayTime - MAX_HOURS/2) / (MAX_HOURS/2) * COLOR_CHANGE_SPEED ) );
		returnedSkyColor[2] = skyColor[2] * ( 1 - ( (dayTime - MAX_HOURS/2) / (MAX_HOURS/2) * COLOR_CHANGE_SPEED ) );
	}
	// above world
	else {
		returnedSkyColor[0] = skyColor[0] * ( dayTime / (MAX_HOURS/2) ) * COLOR_CHANGE_SPEED;
		returnedSkyColor[1] = skyColor[1] * ( dayTime / (MAX_HOURS/2) ) * COLOR_CHANGE_SPEED;
		returnedSkyColor[2] = skyColor[2] * ( dayTime / (MAX_HOURS/2) ) * COLOR_CHANGE_SPEED;
	}

	// cap returned sky color if it goes past the values of sky color
	if(returnedSkyColor[0] > skyColor[0]) {
		returnedSkyColor[0] = skyColor[0];
	}
	if(returnedSkyColor[1] > skyColor[1]) {
		returnedSkyColor[1] = skyColor[1];
	}
	if(returnedSkyColor[2] > skyColor[2]) {
		returnedSkyColor[2] = skyColor[2];
	}

	// make sure returned sky color is always more than given minimum (for when the sun is under world)	
	if(returnedSkyColor[0] < minSkyColor[0]) {
		returnedSkyColor[0] = minSkyColor[0];
	}	
	if(returnedSkyColor[1] < minSkyColor[1]) {
		returnedSkyColor[1] = minSkyColor[1];
	}
	if(returnedSkyColor[2] < minSkyColor[2]) {
		returnedSkyColor[2] = minSkyColor[2];
	}


	// ---
	

	// calculate average (mean) of all sky shading reached
	float averageShade = (returnedSkyColor[0] + returnedSkyColor[1] + returnedSkyColor[2]) / 3;

	// move block shading towards target block shading
	blockShading = averageShade;

	// set block shading to mimimum if it goes under
	if(blockShading < minimumBlockShading) {
		blockShading = minimumBlockShading;
	}

}


// ---


// draws the sky
void draw_sky(unsigned int worldAtlas) {

	// disable depth testing specifically for the sky, so that it is drawn behind everything else
	glDisable(GL_DEPTH_TEST);

	// bind vao
	glBindVertexArray(sunMesh.vao);

	// use shader program
	glUseProgram(sunShaderProgram);

	// bind texture
	glBindTexture(GL_TEXTURE_2D, worldAtlas);

	// get camera matrices
	mat4* model = get_model();
	mat4* view = get_view();
	mat4* proj = get_projection();

	// get camera position
	vec3* camPos = get_camera_pos();


	// ---
	

	// create new model matrix and copy over contents of model matrix to it
	mat4 newModel = GLM_MAT4_ZERO_INIT;
	glm_mat4_copy((*model), newModel);

	// get angle of rotation for the sun (offset backwards by a fourth of a day)
	float angle = glm_rad( dayTime / MAX_HOURS * 360);

	// get sun z and y values
	float z = SUN_DISTANCE * cos(angle);
	float y = SUN_DISTANCE * sin(angle);

	// rotate new model based on current time
	glm_rotate_at(newModel, (vec3){(*camPos)[0], (*camPos)[1]+y, (*camPos)[2]+z}, -angle, (vec3){1.0f, 0.0f, 0.0f});


	// ---


	// get locations of uniform camera matrices
	int modelLoc = glGetUniformLocation(sunShaderProgram, "model");
	int viewLoc = glGetUniformLocation(sunShaderProgram, "view");
	int projLoc = glGetUniformLocation(sunShaderProgram, "proj");

	// get camera position uniform
	int camPosLoc = glGetUniformLocation(sunShaderProgram, "camPos");	

	// get location of underwater uniform
	int uwLoc = glGetUniformLocation(sunShaderProgram, "underWater");

	// get shading color uniform
	int shadingLoc = glGetUniformLocation(sunShaderProgram, "shading");

	// load data into uniforms
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, newModel);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *view);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, *proj);

	// load data into camera position uniform
	glUniform3f(camPosLoc, (*camPos)[0], (*camPos)[1]+y, (*camPos)[2]+z);

	// pass underWaterLevel boolean in the form of an integer to fragment shader
	if(get_under_water_level()) {
		glUniform1i(uwLoc, 1);
	}
	else {
		glUniform1i(uwLoc, 0);
	}

	// send either moon or sun color to shader depending on whether its day or night
	if(isDay) {
		glUniform3f(shadingLoc, sunColor[0]/255, sunColor[1]/255, sunColor[2]/255);
	}
	else {
		glUniform3f(shadingLoc, moonColor[0]/255, moonColor[1]/255, moonColor[2]/255);
	}

	// draw the elements
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// re-enable depth testing to render everything else
	glEnable(GL_DEPTH_TEST);

}

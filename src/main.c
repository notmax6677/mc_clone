#include <GLAD33/glad.h>
#include <GLFW/glfw3.h>
#include <CGLM/cglm.h>

#include <stdio.h>
#include <time.h>

#include "headers/camera.h"
#include "headers/world.h"
#include "headers/pointer.h"

#include "headers/sky.h"


// ---


// window init data
const int initWidth = 800;
const int initHeight = 800;
const char* initTitle = "Minecraft Clone";


// ---


// delta time
float deltaTime = 0.0f;
float lastFrameTime = 0.0f;


// ---


// initiate some stuff
void init(GLFWwindow* window) {
	// randomize maths seed
	srand(time(NULL));

	init_camera(window);

	init_world();
}

// frame update
void update(GLFWwindow* window) {
	// delta time processing
	float currentFrameTime = glfwGetTime(); // get total time
	deltaTime = currentFrameTime - lastFrameTime; // calculate delta time
	lastFrameTime = currentFrameTime; // assign last frame time to current

	update_camera(window, deltaTime);

	update_world(window, deltaTime);
}

// frame draw
void draw(GLFWwindow* window) {

	vec3* skyCol = get_sky_col();
	
	// set color clear buffer
	glClearColor( (float)(*skyCol)[0]/255, (float)(*skyCol)[1]/255, (float)(*skyCol)[2]/255, 1.0f);

	// clear color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_world();
}


// ---


// callback for when keys are pressed, down, released etc
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	// KEY F - lock/unlock cursor
	if(key == GLFW_KEY_F && action == GLFW_PRESS) {

		// reverse cursorLocked boolean
		bool curLocked = get_cursor_lock();
		curLocked = !curLocked;

		// set new cursor lock
		set_cursor_lock(window, curLocked);

		// true
		if(curLocked) {
			// unlock cursor
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
		}
		// false
		else {
			// lock cursor
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
	
	// KEY X - wireframe draw mode
	if(key == GLFW_KEY_X && action == GLFW_PRESS) {

		// get the wireframe mode
		bool wireframeMode = get_wireframe_mode();
		// invert it
		wireframeMode = !wireframeMode;

		// set polygon mode accordingly based on new inverted wireframe mode boolean
		if(wireframeMode) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// set wireframe mode to new inverted mode
		set_wireframe_mode(wireframeMode);

	}

	// KEY N - toggle day/night cycle
	if(key == GLFW_KEY_N && action == GLFW_PRESS) {

		// get current day/night cycle
		bool active = get_day_night_cycle();

		// set it to the inverse of what we got
		set_day_night_cycle(!active);

	}

	// KEY T - toggle drawing water
	if(key == GLFW_KEY_T && action == GLFW_PRESS) {

		// call toggle drawing water method
		toggleDrawingWater();

	}

}

// mouse callback for when the cursor is moved
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	camera_mouse_callback(window, xpos, ypos);
}

// mouse button/input callback
void mouse_input_callback(GLFWwindow* window, int button, int action, int mods) {
	pointer_mouse_input_callback(window, button, action, mods);
}

// callback called upon window resize
void resize_callback(GLFWwindow* window, int width, int height) {
	// set viewport
	glViewport(0, 0, width, height);
}


// ---


// entrance
int main() {

	// initiate glfw
	glfwInit();

	// set context to 3.3 core
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window object
	GLFWwindow* window = glfwCreateWindow(initWidth, initHeight, initTitle, NULL, NULL);

	// return error message and stop program if window creation failed
	if(window == NULL) {
		printf("ERROR: Failed to create GLFW window.");
		glfwTerminate();
		return -1;
	}

	// set opengl context to window
	glfwMakeContextCurrent(window);

	// attempt to init glad and throw error if failed
	if(!gladLoadGLLoader( (GLADloadproc)(glfwGetProcAddress) )) {
		printf("ERROR: Failed to initialize GLAD.");
		return -1;
	}

	// set initial viewport
	glViewport(0, 0, initWidth, initHeight);

	// set key callback
	glfwSetKeyCallback(window, key_callback);

	// set mouse callback
	glfwSetCursorPosCallback(window, mouse_callback);

	// set mouse INPUT callback
	glfwSetMouseButtonCallback(window, mouse_input_callback);

	// set resize window callack
	glfwSetFramebufferSizeCallback(window, resize_callback);


	// ---
	

	// enable depth testing for proper drawing in correct order on z axis
	glEnable(GL_DEPTH_TEST);

	// enable necessary gl blending for transparency support
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// set the line width for wireframe lines
	glLineWidth(2);

	// call init function for other stuff
	init(window);

	// run until window close triggered
	while(!glfwWindowShouldClose( window )) {

		// main loop

		update(window);

		draw(window);

		// swap buffers and poll for events
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	// terminate glfw at end of program
	glfwTerminate();

	return 0;
}

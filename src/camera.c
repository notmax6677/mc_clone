#include <GLAD33/glad.h>
#include <GLFW/glfw3.h>
#include <CGLM/cglm.h>

// settings
float fov = 45.0f; // field of view (in degrees)
float nearZ = 0.1f; // closest point to render at
float farZ = 200.0f; // furthest point to render at

float cameraSpeed = 10.0f; // speed of camera movement

float sensitivity = 0.1f;
bool cursorLocked = false; // if cursor is locked or not
float cursorLastX, cursorLastY; // cursor last frame coordinates


// fov values for when zooming or not zooming
const float NORMAL_FOV = 45.0f;
const float ZOOM_FOV = 20.0f;

// whether or not player is zooming
bool zooming = false;


// camera pitch + yaw
float pitch = 0.0f;
float yaw = 0.0f;

// layer matrices for camera
mat4 model = GLM_MAT4_IDENTITY_INIT;
mat4 view = GLM_MAT4_IDENTITY_INIT;
mat4 projection = GLM_MAT4_IDENTITY_INIT;

// vectors for camera control
vec3 cameraPos = GLM_VEC3_ZERO_INIT;
vec3 cameraFront = GLM_VEC3_ZERO_INIT;
vec3 cameraUp = GLM_VEC3_ZERO_INIT;


// ---


// last recorded widtha and height of window
int lastRecordedWidth = 0;
int lastRecordedHeight = 0;


// ---


// centres the camera by positioning it in the centre of the world
void centre_cam_pos(int world_size, int chunk_width, int chunk_length) {
	cameraPos[0] = (world_size/2) * chunk_width;  // x
	cameraPos[2] = (world_size/2) * chunk_length; // z
}


// ---


// toggles the zoom of the camera
void toggle_zoom() {
	// inverse zoom boolean
	zooming = !zooming;

	// change fov value based on zooming bool
	if(zooming) {
		fov = ZOOM_FOV;
	}
	else {
		fov = NORMAL_FOV;
	}

	// update projection matrix
	glm_perspective( glm_rad(fov), (float)lastRecordedWidth/lastRecordedHeight, nearZ, farZ, projection );
}


// ---


// callback for when the window is resized, but regarding specifically camera related processes
void camera_resize_callback(int width, int height) {
	// update projection matrix
	glm_perspective( glm_rad(fov), (float)width/height, nearZ, farZ, projection );

	// update last recorded width/height
	lastRecordedWidth = width;
	lastRecordedHeight = height;
}


// ---


// function for mouse callback, but specific to camera functions, this is called in the actual mouse callback in main.c
void camera_mouse_callback(GLFWwindow* window, double xpos, double ypos) {

	// only run if cursor is locked onto window
	if(cursorLocked) {
		
		// calculate mouse cursor offsets
		float xOffset = xpos - cursorLastX;
		float yOffset = ypos - cursorLastY;

		// multiply offsets by sensitivity
		xOffset *= sensitivity;
		yOffset *= sensitivity;

		// set last coordinates to current coordinates for next frame
		cursorLastX = xpos;
		cursorLastY = ypos;

		
		// ---
			

		// apply offsets to yaw and pitch
		yaw += xOffset;
		pitch -= yOffset;

		// cap pitch to <90 so that you dont get weird camera movements when you look too up
		if(pitch > 89.0f) {
			pitch = 89.0f;
		}
		if(pitch < -89.0f) {
			pitch = -89.0f;
		}
		

		// create direction vector
		vec3 direction = GLM_VEC3_ZERO_INIT;

		// set x
		direction[0] = cos(glm_rad( yaw )) * cos(glm_rad( pitch ));
		// set y
		direction[1] = sin(glm_rad( pitch ));
		// set z
		direction[2] = sin(glm_rad( yaw )) * cos(glm_rad( pitch ));

		// create normalized copy of direction vector
		vec3 normalized;
		glm_vec3_copy(direction, normalized);
		glm_normalize(normalized);

		// write normalized value to camera front vector
		glm_vec3_copy(normalized, cameraFront);

	}


}


// ---


// process inputs for camera
void process_camera_inputs(GLFWwindow* window, float deltaTime) {

	// KEY W - forward
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {

		// calculate and store product of camera speed and camera front
		vec3 product;
		glm_vec3_mul((vec3){cameraFront[0] / cos(glm_rad(pitch)), 0, cameraFront[2] / cos(glm_rad(pitch))}, 
				(vec3){cameraSpeed*deltaTime, 0, cameraSpeed*deltaTime}, product);

		// add product to camera position
		glm_vec3_add(cameraPos, product, cameraPos);

	}

	// KEY S - backward
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {

		// calculate and store product of camera speed and camera front
		vec3 product;
		glm_vec3_mul((vec3){cameraFront[0] / cos(glm_rad(pitch)), 0, cameraFront[2] / cos(glm_rad(pitch))}, 
				(vec3){cameraSpeed*deltaTime, 0, cameraSpeed*deltaTime}, product);

		// subtract product from camera position
		glm_vec3_sub(cameraPos, product, cameraPos);

	}

	// KEY A - left
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {

		// calculate and store cross value
		vec3 cross;
		glm_cross((vec3){cameraFront[0] / cos(glm_rad(pitch)), 0, cameraFront[2] / cos(glm_rad(pitch))}, cameraUp, cross);

		// then normalized value
		vec3 normalized;
		glm_vec3_copy(cross, normalized);
		glm_normalize(normalized);

		// now finally get product
		vec3 product;
		glm_vec3_mul(normalized, (vec3){cameraSpeed*deltaTime, 0, cameraSpeed*deltaTime}, product);

		// and subtract the product from camera position
		glm_vec3_sub(cameraPos, product, cameraPos);

	}

	// KEY D - right
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {

		// calculate and store cross value
		vec3 cross;
		glm_cross((vec3){cameraFront[0] / cos(glm_rad(pitch)), 0, cameraFront[2] / cos(glm_rad(pitch))}, cameraUp, cross);

		// then normalized value
		vec3 normalized;
		glm_vec3_copy(cross, normalized);
		glm_normalize(normalized);

		// now finally get product
		vec3 product;
		glm_vec3_mul(normalized, (vec3){cameraSpeed*deltaTime, 0, cameraSpeed*deltaTime}, product);

		// and add the product to camera position
		glm_vec3_add(cameraPos, product, cameraPos);

	}


	// ---
	

	// KEY SPACE - up
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {

		// add up vector with magnitude cameraSpeed * deltaTime to camera position
		glm_vec3_add(cameraPos, (vec3){0, cameraSpeed*deltaTime, 0}, cameraPos);

	}

	// KEY LEFT SHIFT - down
	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {

		// subtract vector with magnitude cameraSpeed * deltaTime from camera position
		glm_vec3_sub(cameraPos, (vec3){0, cameraSpeed*deltaTime, 0}, cameraPos);

	}

	// KEY LEFT CTRL - sprint
	if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		cameraSpeed = 20.0f; // set camera speed to higher value
	}
	else {
		cameraSpeed = 10.0f; // otherwise restore to normal lower value
	}

}


// ---


// initiate camera related variables and/or functionality
void init_camera(GLFWwindow* window) {

	// get window size
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// initiate projection matrix
	glm_perspective( glm_rad(fov), (float)width/height, nearZ, farZ, projection);

	// set initial values of last recorded width/height
	lastRecordedWidth = width;
	lastRecordedHeight = height;

	// initiate cursor last positions by setting in the middle of the screen
	cursorLastX = (float)width/2;
	cursorLastY = (float)height/2;


	// ---
	

	// set initial value of camera front
	// adding to it is the same as setting it because it initiates with values (0, 0, 0)
	glm_vec3_add(cameraFront, (vec3){0.0f, 0.0f, -1.0f}, cameraFront);

	// set initial value of camera up
	// adding to it is the same as setting it because it initiates with values (0, 0, 0)
	glm_vec3_add(cameraUp, (vec3){0.0f, 1.0f, 0.0f}, cameraUp);


	// ---
	

	// apply first camera look direction to camera front based on initial pitch and yaw

	// cap pitch to <90 so that you dont get weird camera movements when you look too up
	if(pitch > 89.0f) {
		pitch = 89.0f;
	}
	if(pitch < -89.0f) {
		pitch = -89.0f;
	}
	

	// create direction vector
	vec3 direction = GLM_VEC3_ZERO_INIT;

	// set x
	direction[0] = cos(glm_rad( yaw )) * cos(glm_rad( pitch ));
	// set y
	direction[1] = sin(glm_rad( pitch ));
	// set z
	direction[2] = sin(glm_rad( yaw )) * cos(glm_rad( pitch ));

	// create normalized copy of direction vector
	vec3 normalized;
	glm_vec3_copy(direction, normalized);
	glm_normalize(normalized);

	// write normalized value to camera front vector
	glm_vec3_copy(normalized, cameraFront);

	// set initial cam pos
	cameraPos[1] = 64;

}

// update camera processes
void update_camera(GLFWwindow* window, float deltaTime) {
	// process camera inputs for movement
	process_camera_inputs(window, deltaTime);


	// ---
	

	// sum vector
	vec3 sum;
	// calculate sum of position and front and load it into sum vector
	glm_vec3_add(cameraPos, cameraFront, sum);
	// execute lookat function and write value to view matrix
	glm_lookat(cameraPos, sum, cameraUp, view);
}


// ---


// getters

mat4* get_model() {
	return &model;
}
mat4* get_view() {
	return &view;
}
mat4* get_projection() {
	return &projection;
}

vec3* get_camera_pos() {
	return &cameraPos;
}
vec3* get_camera_front() {
	return &cameraFront;
}

bool get_cursor_lock() {
	return cursorLocked;
}

// setters

void set_model(mat4* matrix) {
	glm_mat4_copy(*matrix, model);
}
void set_view(mat4* matrix) {
	glm_mat4_copy(*matrix, view);
}
void set_projection(mat4* matrix) {
	glm_mat4_copy(*matrix, projection);
}

void set_cursor_lock(GLFWwindow* window, bool lock) {
	cursorLocked = lock;

	// get window size
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// if lock is true
	if(lock) {
		glfwSetCursorPos(window, (float)width/2, (float)height/2);

		cursorLastX = (float)width/2;
		cursorLastY = (float)height/2;
	}
}

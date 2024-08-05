#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec2 chunkOffset;

uniform float tide;

uniform vec3 camPos;

uniform int fog;

out vec3 col;
out vec2 texCoord;
out float opacity;

float OPACITY_MULTIPLIER = 3.0;

float MAXIMUM_OPACITY_DISTANCE = 35.0;

void main() {
	gl_Position = proj * view * model * vec4(aPos.x + chunkOffset.x, aPos.y + tide, aPos.z + chunkOffset.y, 1.0);

	col = aCol;
	texCoord = aTexCoord;

	if(fog == 1) {
		float xOp = 1 - abs(camPos.x - (aPos.x+chunkOffset.x)) / MAXIMUM_OPACITY_DISTANCE;
		float yOp = 1 - abs(camPos.y - aPos.y) / MAXIMUM_OPACITY_DISTANCE;
		float zOp = 1 - abs(camPos.z - (aPos.z+chunkOffset.y)) / MAXIMUM_OPACITY_DISTANCE;

		opacity = (( xOp + yOp + zOp ) / 3) * OPACITY_MULTIPLIER;

		if(opacity > 1.0) {
			opacity = 1.0;
		}
	}
	else {
		opacity = 1.0;
	}
}

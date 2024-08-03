#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 camPos;

out vec2 texCoord;

void main() {
	gl_Position = proj * view * model * 
		vec4(aPos.x + camPos.x, aPos.y + camPos.y, aPos.z + camPos.z, 1.0);

	texCoord = aTexCoord;
}

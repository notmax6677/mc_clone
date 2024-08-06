#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aTexCoord;

uniform vec2 windowSize;

out vec3 col;
out vec2 texCoord;

void main() {
	gl_Position = vec4(aPos.x / (windowSize.x/windowSize.y), aPos.y, aPos.z, 1.0);

	col = aCol;
	texCoord = aTexCoord;
}

#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec2 chunkOffset;

out vec3 col;
out vec2 texCoord;

void main() {
	gl_Position = proj * view * model * vec4(aPos.x + chunkOffset.x, aPos.y, aPos.z + chunkOffset.y, 1.0);

	col = aCol;
	texCoord = aTexCoord;
}

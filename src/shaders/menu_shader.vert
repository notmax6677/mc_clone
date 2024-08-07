#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform vec2 windowSize;

uniform vec2 offset;

out vec2 texCoord;

void main() {
	gl_Position = vec4((aPos.x + offset.x) * (windowSize.y/windowSize.x), aPos.y + offset.y, 0.0, 1.0);

	texCoord = aTexCoord;
}

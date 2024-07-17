#version 330 core

out vec4 FragColor;

in vec3 col;
in vec2 texCoord;

uniform sampler2D inTexture;

void main() {
	FragColor = texture(inTexture, texCoord);
}

#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform int underWater;

uniform vec3 shading;

uniform float opacity;

float waterR = 0.2;
float waterG = 0.2;
float waterB = 0.4;

uniform sampler2D inTexture;

void main() {
	if(underWater == 1) {
		FragColor = texture(inTexture, texCoord) * vec4(0.5, 0.5, 0.5, 1.0) * vec4(waterR, waterG, waterB, 1.0) * vec4(1.0, 1.0, 1.0, opacity);
	}
	else {
		FragColor = texture(inTexture, texCoord) * vec4(shading.x, shading.y, shading.z, 1.0) * vec4(1.0, 1.0, 1.0, opacity);
	}
}

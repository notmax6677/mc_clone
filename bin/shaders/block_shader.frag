#version 330 core

out vec4 FragColor;

in vec3 col;
in vec2 texCoord;

uniform int underWater;

uniform float shading;

float waterR = 0.2;
float waterG = 0.2;
float waterB = 0.6;

uniform sampler2D inTexture;

void main() {
	if(underWater == 1) {
	                 // texture color
		FragColor = ( texture(inTexture, texCoord) * vec4(col, 1.0) ) * vec4(0.5, 0.5, 0.5, 1.0) * vec4(waterR, waterG, waterB, 1.0) * vec4(shading/255, shading/255, shading/255, 1.0) * 2;
	}
	else {
		FragColor = texture(inTexture, texCoord) * vec4(col, 1.0) * vec4(shading/255, shading/255, shading/255, 1.0);
	}
}

#version 450

// in values
layout(location = 0) in vec3 fragColor;

// out values
layout(location = 0) out vec4 outColor;

void main() {
	outColor = vec4(fragColor, 1.0);
}
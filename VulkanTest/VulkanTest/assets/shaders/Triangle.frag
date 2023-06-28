#version 460

// in values
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

// uniform values
layout(binding = 1) uniform sampler2D texSampler;

// out values
layout(location = 0) out vec4 outColor;

void main() {
	outColor = texture(texSampler, fragTexCoord);
}
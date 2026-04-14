#version 330 core

layout(location=0) out vec4 color;

uniform vec3 lightColor;

void main() {
	color = vec4(lightColor * 10.0f, 1.0);
}
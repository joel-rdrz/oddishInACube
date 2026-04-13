#version 330 core

in vec2 uv;

out vec2 outSum;

uniform sampler2D vsm;
uniform vec2 blurDir;

void main() {
	outSum = texture(vsm, uv).rg + texture(vsm, uv - blurDir).rg;
}
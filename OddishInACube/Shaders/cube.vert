#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat3 normalMatrix;
uniform mat4 model;

out vec3 cameraSpacePos;
out vec3 normalToCamera;
out float worldY;

void main(){

	gl_Position = mvp * vec4(pos, 1);

	normalToCamera = normalize(normalMatrix * normal);

	cameraSpacePos = (mv * vec4(pos, 1.0)).xyz;

	worldY = (model * vec4(pos, 1.0)).y;

}
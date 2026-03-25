#version 330 core

layout(location=0) in vec3 pos;
layout(location = 1) in vec3 normal;

uniform mat4 planeMVP;
uniform mat3 normalMatrix;
uniform mat4 mv;

out vec3 cameraSpacePos;
out vec3 normalToCamera;

uniform mat4 matrixShadow;
out vec4 lightView_Position;

void main(){

	gl_Position = planeMVP * vec4(pos, 1.0);

	normalToCamera = normalize(normalMatrix * normal);

	cameraSpacePos = (mv * vec4(pos, 1.0)).xyz;

	lightView_Position = matrixShadow * vec4(pos, 1);

}
#version 330 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 txc;


uniform mat4 mvp;
uniform mat3 normalMatrix;
uniform mat4 mv;

out vec3 cameraSpacePos;
out vec3 normalToCamera;
out vec2 texCoord;

uniform mat4 matrixShadow;

out vec4 lightView_Position;


void main(){
	gl_Position = mvp * vec4(pos, 1);
	normalToCamera = normalize(normalMatrix * normal);

	cameraSpacePos = (mv * vec4(pos, 1.0)).xyz;

	lightView_Position = matrixShadow * vec4(pos, 1);
	texCoord = vec2(txc.x, 1.0 - txc.y);
}
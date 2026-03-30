#version 330 core

layout(location = 0) out vec4 color;

in vec3 normalToCamera;
in vec3 cameraSpacePos;

void main(){

	vec3 norm = normalize(normalToCamera);
	vec3 view = normalize(-cameraSpacePos);

	//make edges more opaque
	float glassEdge = pow(1.0 - max(dot(norm, view), 0.0), 2.0);

	//mix between light blue and white color depending on the angle of the surface to the camera
	vec3 glassColor = mix(vec3(0.8, 0.95, 1.0), vec3(1.0), glassEdge);
	float alpha = mix(0.05, 0.4, glassEdge);

	color = vec4(glassColor, alpha);

}
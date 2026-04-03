#version 330 core

layout(location = 0) out vec4 color;

in vec3 normalToCamera;
in vec3 cameraSpacePos;
in float worldY;
uniform float waterHeight;
void main(){

	vec3 norm = normalize(normalToCamera);
	vec3 view = normalize(-cameraSpacePos);

	//make edges more opaque
	float glassEdge = pow(1.0 - max(dot(norm, view), 0.0), 2.0);

	//mix between light blue and white color depending on the angle of the surface to the camera
	vec3 glassColor = mix(vec3(0.8, 0.95, 1.0), vec3(1.0), glassEdge);
	float alpha = mix(0.05, 0.4, glassEdge);

	if(worldY < waterHeight) 
	{
		vec3 waterColor = vec3(0.1, 0.4, 0.7);
		glassColor = mix(glassColor, waterColor, 0.85);
		alpha = mix(0.4, 0.8, glassEdge);

	}
	color = vec4(glassColor, alpha);

}
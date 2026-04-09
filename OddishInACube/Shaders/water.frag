#version 330 core

layout(location=0) out vec4 color;

in vec3 fragNormal;
in vec3 fragWorldPos;

void main() 
{
	// Fake lightDIr the actual lightDIr is done in the lighting shader but we need it here to calculate the color of the water
	// TODO: make this more accurate by passing the lightDir from the lighting shader to this shader
	vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
	float diffuse = max(dot(fragNormal, lightDir), 0.0);
	vec3 ambientColor = vec3(0.0, 0.2, 0.5);
	vec3 sunlitColor = vec3(0.1, 0.5, 0.9);

	vec3 finalColor = ambientColor + diffuse * sunlitColor;

	color = vec4(finalColor, 1.0);

}
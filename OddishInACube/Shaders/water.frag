#version 330 core

layout(location=0) out vec4 color;

in vec3 fragNormal;
in vec3 fragWorldPos;

uniform vec3 lightDir;

void main() 
{
	vec3 lD = normalize(lightDir);
	float diffuse = max(dot(fragNormal, lD), 0.0);
	vec3 ambientColor = vec3(0.0, 0.2, 0.5);
	vec3 sunlitColor = vec3(0.1, 0.5, 0.9);

	vec3 finalColor = ambientColor + diffuse * sunlitColor;

	color = vec4(finalColor, 1.0);

}
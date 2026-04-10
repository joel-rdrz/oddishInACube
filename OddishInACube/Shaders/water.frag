#version 330 core

layout(location=0) out vec4 color;

in vec3 fragNormal;
in vec3 fragWorldPos;

uniform vec3 lightDir;

uniform vec3 cameraPos;

uniform float screenWidth;
uniform float screenHeight;

uniform sampler2D reflectionTex;
uniform sampler2D refractionTex;

void main() 
{
	vec3 n = normalize(fragNormal);
	vec3 lD = normalize(lightDir);

	vec3 viewDir = normalize(cameraPos - fragWorldPos);
	vec3 reflectDir = reflect(-lD, n);

	float fresnal = 0.2 + 0.8 * pow(1.0 - max(dot(viewDir, n), 0.0), 5.0);

	float specular = pow(max(dot(viewDir, reflectDir), 0.0), 128.0);
	vec3 glare = vec3(1.0) * specular * 1.5;

	vec2 screenCoords = gl_FragCoord.xy / vec2(screenWidth, screenHeight);
	vec2 distoredCoords = screenCoords - (n.xz * 0.05);
	distoredCoords = clamp(distoredCoords, 0.001, 0.999);

	vec3 waterColor = texture(refractionTex, distoredCoords).rgb;
	vec3 skyColor = texture(reflectionTex, distoredCoords).rgb;

	vec3 finalColor = mix(waterColor, skyColor, fresnal);

	finalColor += glare;


	color = vec4(finalColor, 0.8);

}
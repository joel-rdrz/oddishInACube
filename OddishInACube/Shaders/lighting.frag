#version 330 core

layout(location=0) out vec4 color;

in vec3 normalToCamera;
in vec3 cameraSpacePos;
in vec2 texCoord;

uniform vec3 lightPos;
vec3 lightColor = vec3(1.0,1.0,1.0);

uniform sampler2DShadow shadow;
uniform sampler2D tex;

in vec4 lightView_Position;

void main(){

	//ambience
	float ambienceIntensity = 0.1;
	vec3 ambience = lightColor * ambienceIntensity;
	//diffuse
	vec3 norm = normalize(normalToCamera);
	vec3 lD = normalize(lightPos - cameraSpacePos);
	vec3 diffuse = max(dot(norm, lD), 0.0) * lightColor;

	//specular 
	vec3 view = normalize(-cameraSpacePos);
	vec3 h = normalize(lD + view);
	vec3 specular = pow(max(dot(norm, h), 0.0), 16) * lightColor;

	vec3 textureColor = texture(tex, texCoord).rgb;

	//put it all together

	float shadowTerm = textureProj(shadow, lightView_Position);
	
	vec3 final = (ambience + (shadowTerm * diffuse)) * textureColor + (shadowTerm * specular);
	color = vec4(final, 1);


}
#version 330 core

layout(location=0) out vec4 color;

uniform sampler2D shadow;
uniform vec3 lightPos;

vec3 lightColor = vec3(1.0,1.0,1.0);

in vec3 normalToCamera;
in vec3 cameraSpacePos;
in vec4 lightView_Position;

float VSMShadow(vec4 lightViewPos) {
	vec3 coord = lightViewPos.xyz / lightViewPos.w;
	vec2 uv = coord.xy;
	float t = coord.z;
	
	float r = 0.02; 
	
	vec2 tl = texture(shadow, uv + vec2(-r, -r)).rg; 
	vec2 tr = texture(shadow, uv + vec2( r, -r)).rg; 
	vec2 bl = texture(shadow, uv + vec2(-r,  r)).rg;
	vec2 br = texture(shadow, uv + vec2( r,  r)).rg;
	
	float filterSize = (r * 2.0) * 1024.0;
	float area = filterSize * filterSize;
	vec2 moments = abs(br - tr - bl + tl) / area;
	
	if (t <= moments.x) return 0.0;

	float variance = max(moments.y - (moments.x * moments.x), 0.00002);
	
	float d = t - moments.x;

	float p_max = variance / (variance + d*d);
	
	return 1.0 - p_max;
}

void main(){
	if (!gl_FrontFacing) {
		color = vec4(0.2, 0.2, 0.2, 1.0);
		return;
	}

	float ambienceIntensity = 0.1;
	vec3 ambience = lightColor * ambienceIntensity;
	
	vec3 norm = normalize(normalToCamera);
	vec3 lD = normalize(lightPos - cameraSpacePos);
	vec3 diffuse = max(dot(norm, lD), 0.0) * lightColor;
	
	vec3 view = normalize(-cameraSpacePos);
	vec3 h = normalize(lD + view);
	vec3 specular = pow(max(dot(norm, h), 0.0), 32) * lightColor;

	float shadowTerm = VSMShadow(lightView_Position);
	
	vec3 final = (ambience + (shadowTerm * diffuse)) * vec3(0.2, 0.2, 0.2) + (shadowTerm * specular);
	
	color = vec4(final, 1.0);
}
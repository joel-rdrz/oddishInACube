#version 330 core

layout(location=0) in vec3 pos;

uniform mat4 mvp;
uniform mat4 projectorMatrix;
uniform float waterHeight;
// nandhini if u read this later cause u need to do some shadow stuff this is a lot like our ray tracing assign 
// from 4600
void main() 
{
	// FIrst transformation z = -1.0
	vec4 nearClip = vec4(pos.x, pos.y, -1.0, 1.0);
	vec4 nearWorldHomog = projectorMatrix * nearClip;
	// 2.3 in the docoumentation, we need to divide by w to get the world coordinates
	vec3 nearWorld = nearWorldHomog.xyz / nearWorldHomog.w; 

	// Second transformation z = 1.0
	vec4 farClip = vec4(pos.x, pos.y, 1.0, 1.0);
	vec4 farWorldHomog = projectorMatrix * farClip;
	// 2.3 in the docoumentation, we need to divide by w to get the world coordinates
	vec3 farWorld = farWorldHomog.xyz / farWorldHomog.w; 

	// 3. Create ray and solve for t
	vec3 rayDir = normalize(farWorld - nearWorld);
	float t = (waterHeight - nearWorld.y) / rayDir.y;

	if (t < 0.0)
	{
		gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
		return;
	}

	vec3 worldPos = nearWorld + rayDir * t;
	gl_Position = mvp * vec4(worldPos, 1.0);
}
#version 330 core

layout(location=0) in vec3 pos;

uniform mat4 mvp;
uniform mat4 projectorMatrix;
uniform float waterHeight;
uniform float time;

out vec3 fragNormal;
out vec3 fragWorldPos;

// nandhini if u read this later cause u need to do some shadow stuff this is a lot like our ray tracing assign 
// from 4600
float getWaveHeight(float x, float z) {
	float waveHeight = sin(x * 0.5 + time) * 0.5;
	waveHeight += cos(z * 0.4 + time * 1.2) * 0.5;
	return waveHeight;
}
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

    worldPos.y += getWaveHeight(worldPos.x, worldPos.z);

	// Nandhini this is finite differences!! 3200 coming in handy
	float deltaT = 0.1; // Small time step for numerical differentiation
	float heightLeft = getWaveHeight(worldPos.x - deltaT, worldPos.z);
	float heightRight = getWaveHeight(worldPos.x + deltaT, worldPos.z);
	float heightDown = getWaveHeight(worldPos.x, worldPos.z - deltaT);
	float heightUp = getWaveHeight(worldPos.x, worldPos.z + deltaT);

	// Get our actual direvatives
	vec3 dx = vec3(deltaT * 2.0, heightRight - heightLeft, 0.0);
	vec3 dz = vec3(0.0, heightUp - heightDown, deltaT * 2.0);

	// Cross product to get the normal, this is what it said to do in the paper, and it makes sense because the normal is
	// perpendicular to the surface, and the surface is defined by these two vectors
	fragNormal = normalize(cross(dz, dx));
	fragWorldPos = worldPos;

	gl_Position = mvp * vec4(worldPos, 1.0);
}
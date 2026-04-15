#version 330 core

layout(location=0) in vec3 pos;

uniform mat4 mvp;
uniform mat4 projector;
uniform float waterHeight;
uniform float time;

out vec3 fragNormal;
out vec3 fragWorldPos;

uniform sampler2D noiseTex;
uniform float scale;

// nandhini if u read this later cause u need to do some shadow stuff this is a lot like our ray tracing assign 
// from 4600
float getWaveHeight(float x, float z) {
	vec2 worldXZ = vec2(x,z);

	//This is how the paper does the wave height (pg 38), it uses a noise texture and samples it at different
	// frequencies and amplitudes to create a more realistic wave pattern The paper doesn't use time instead
	// it makes a 3d perlin noise function every frame. That is a whole other project that the paper mostly brushes
	// over, but I think using a noise texture is a good enough approximation for our purposes, and it is much easier to implement

	vec2 tc0 = (scale * worldXZ * 0.015625) + (time * 0.02);
	vec2 tc1 = (scale * worldXZ * 0.25) + (time * 0.05);

	float h = texture(noiseTex, tc0).r + texture(noiseTex, tc1).r - 0.5;

	return h;

}
void main() 
{
	// FIrst transformation z = -1.0
	vec4 nearClip = vec4(pos.x, pos.y, -1.0, 1.0);
	vec4 nearWorldHomog = projector * nearClip;
	// 2.3 in the docoumentation, we need to divide by w to get the world coordinates
	vec3 nearWorld = nearWorldHomog.xyz / nearWorldHomog.w; 

	// Second transformation z = 1.0
	vec4 farClip = vec4(pos.x, pos.y, 1.0, 1.0);
	vec4 farWorldHomog = projector * farClip;
	// 2.3 in the docoumentation, we need to divide by w to get the world coordinates
	vec3 farWorld = farWorldHomog.xyz / farWorldHomog.w; 
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
	float deltaT = 0.2; 
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
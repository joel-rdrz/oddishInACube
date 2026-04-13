#ifndef SHADOWMAPPING_H
#define SHADOWMAPPING_H
#include <GL/glew.h>
#include <cyTriMesh.h>
#include <cyMatrix.h>
#include <cyGL.h>

// ShadowMapping implements Summed-Area Variance Shadow Maps (SAVSM)in 4 steps:
//	1. Render scene moments (depth, depth^2) into VSM texture from light's POV
//	2. Build Summed-Area Table (SAT) through 10 horizontal + 10 vertical prefix sum passes
//	3. Lighting shader samples 4 SAT corners to get average moments over filter region
//	4. Chebyshev inequality converts moments into soft shadow probability
class ShadowMapping {
public:
	cy::GLSLProgram shadowProg;
	cy::GLSLProgram blurProg;

	cy::GLRenderTexture2D shadowMap;
	cy::GLRenderTexture2D blurMap;

	int shadowWidth = 1024;
	int shadowHeight = 1024;

	// Light-space matrices that are public so LightingHandler can use them for plane shadows
	cy::Matrix4f lightView;
	cy::Matrix4f lightProjMatrix;
	cy::Matrix4f mlp;
	cy::Matrix4f T;
	cy::Matrix4f S;
	cy::Matrix4f matrixShadow;

	// SAT ping-pong buffers that alternate between these across 20 prefix sum passes
	// After all passes, one contains the complete Summed-Area Table (SAT - blurMap)
	cy::GLRenderTexture2D satPing;
	cy::GLRenderTexture2D satPong;

	GLuint finalSAT;

	void Initialize();
	void RenderShadowPass(cy::Matrix4f fullRotaion, GLuint vao, cy::TriMesh mesh);

private:
	GLuint quadVAO, quadVBO;
};

#endif
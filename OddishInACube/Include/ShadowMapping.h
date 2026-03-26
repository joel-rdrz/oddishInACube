#pragma once
#include <GL/glew.h>
#include <cyMatrix.h>
#include <cyGL.h>

class ShadowMapping {
public:
	cy::GLRenderDepth2D shadowMap;
	cy::GLSLProgram shadowProg;

	int shadowWidth = 1024;
	int shadowHeight = 1024;
	// These are made public because main.cpp needs them. (Changes to main.ccp should be made really carefully and please message the gc when u do :p)
	cy::Matrix4f lightView;
	cy::Matrix4f lightProjMatrix;
	cy::Matrix4f mlp;
	cy::Matrix4f T;
	cy::Matrix4f S;
	cy::Matrix4f matrixShadow;

	void Initialize();

	void RenderShadowPass(cy::Matrix4f fullRotaion, GLuint vao, unsigned int meshNF);
};
#ifndef WATERHANDLER_H
#define WATERHANDLER_H
#include <GL/glew.h>
#include <cyMatrix.h>
#include <cyGL.h>
#include <vector>

class WaterHandler {
public:
	cy::GLSLProgram waterProg;
	cy::GLSLProgram maskProg;
	GLuint waterVao;
	GLuint waterVbo;
	unsigned int vertexCount;

	void Initialize(float waterHeight);

	void RenderWater(cy::Matrix4f projMatrix, cy::Matrix4f viewMatrix, float worldWaterHeight, float time);

	void RenderMask(cy::Matrix4f mvp, GLuint maskvao);
};
#endif
#ifndef WATERHANDLER_H
#define WATERHANDLER_H
#include <GL/glew.h>
#include <cyMatrix.h>
#include <cyGL.h>
#include <vector>

class WaterHandler {
public:
	cy::GLSLProgram waterProg;
	GLuint waterVao;
	GLuint waterVbo;
	unsigned int vertexCount;

	void Initialize(float waterHeight);

	void RenderWater(cy::Matrix4f mvp);
};
#endif
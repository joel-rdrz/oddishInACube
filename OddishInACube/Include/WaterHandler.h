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
	cyGLTexture2D noiseTex;
	cy::GLRenderTexture2D reflectionFBO;
	cy::GLRenderTexture2D refractionFBO;
	GLuint waterVao;
	GLuint waterVbo;
	unsigned int vertexCount;

	void Initialize(float waterHeight);

	void RenderWater(cy::Matrix4f projMatrix, cy::Matrix4f viewMatrix, float worldWaterHeight, float time, 
		cy::Vec3f lightDir, cy::Vec3f cameraPos, float screenWidth, float screenHeight);

	void RenderMask(cy::Matrix4f mvp, GLuint maskvao);

	void initFrameBuffers(int width, int height);
};
#endif
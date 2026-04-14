#ifndef LIGHTINGHANDER_H
#define LIGHTINGHANDLER_H
#include <GL/glew.h>
#include <cyTriMesh.h>
#include <cyMatrix.h>
#include <cyGL.h>

class LightingHandler {
public:
	cy::GLSLProgram prog;
	cy::GLSLProgram planeProg;
	cyGLTexture2D tex;
	cyGLTexture2D ltc1Texture;
	cyGLTexture2D ltc2Texture;
	cy::Vec4f areaLightVertices[4];
	cy::GLSLProgram lightVisualProgram;
	GLuint lightVao;
	GLuint lightVbo;

	void Initialize();

	void RenderLightingPass(
		cy::Matrix4f mvp, cy::Matrix3f normalMatrix, cy::Matrix4f mv, cy::Matrix4f matrixShadow,
		cy::Matrix4f translationMatrix, cy::Matrix4f cameraRot, int screenWidth, int screenHeight,
		GLuint vao, cy::TriMesh mesh, cy::Matrix4f projMatrix, cy::Matrix4f lightProjMatrix,
		cy::Matrix4f lightView, cy::Matrix4f T, cy::Matrix4f S, GLuint planeVao
	);

};

#endif
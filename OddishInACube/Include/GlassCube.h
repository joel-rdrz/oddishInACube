#ifndef GLASSCUBE_H
#define GLASSCUBE_H
#include <GL/glew.h>
#include <cyTriMesh.h>
#include <cyMatrix.h>
#include <cyGL.h>

class GlassCube {
public:
	cy::GLSLProgram glassProg;
	GLuint cubeVao, cubeVbo, cubeEbo;
	cy::Matrix4f cubeModel;

	void Initialize();

	void SetModel();

	void Render(cy::Matrix4f projMatrix, cy::Matrix4f translationMatrix, cy::Matrix4f cameraRot);
};

#endif
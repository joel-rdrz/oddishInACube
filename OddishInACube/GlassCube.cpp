#include "GlassCube.h"

// positions and normals
static float cubeVertices[] = {
	-1,-1, 1,  0, 0, 1,   1,-1, 1,  0, 0, 1,   1, 1, 1,  0, 0, 1,  -1, 1, 1,  0, 0, 1,
	 1,-1,-1,  0, 0,-1,  -1,-1,-1,  0, 0,-1,  -1, 1,-1,  0, 0,-1,   1, 1,-1,  0, 0,-1,
	-1,-1,-1, -1, 0, 0,  -1,-1, 1, -1, 0, 0,  -1, 1, 1, -1, 0, 0,  -1, 1,-1, -1, 0, 0,
	 1,-1, 1,  1, 0, 0,   1,-1,-1,  1, 0, 0,   1, 1,-1,  1, 0, 0,   1, 1, 1,  1, 0, 0,
	-1, 1, 1,  0, 1, 0,   1, 1, 1,  0, 1, 0,   1, 1,-1,  0, 1, 0,  -1, 1,-1,  0, 1, 0,
	-1,-1,-1,  0,-1, 0,   1,-1,-1,  0,-1, 0,   1,-1, 1,  0,-1, 0,  -1,-1, 1,  0,-1, 0
};

static unsigned int cubeIndices[] = {
	 0, 1, 2, 0, 2, 3,
	 4, 5, 6, 4, 6, 7,
	 8, 9, 10, 8, 10, 11,
	12, 13, 14, 12, 14, 15,
	16, 17, 18, 16, 18, 19,
	20, 21, 22, 20, 22, 23
};

void GlassCube::SetModel()
{
	cubeModel = cy::Matrix4f::Translation(cy::Vec3f(0.0f, 20.1f, 0.0f)) * cy::Matrix4f::Scale(cy::Vec3f(23.0f, 23.0f, 23.0f));
}

void GlassCube::Initialize()
{
	glassProg.BuildFiles("Shaders/cube.vert", "Shaders/cube.frag");

	glGenVertexArrays(1, &cubeVao);
	glBindVertexArray(cubeVao);

	glGenBuffers(1, &cubeVbo);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &cubeEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void GlassCube::Render(cy::Matrix4f projMatrix, cy::Matrix4f translationMatrix, cy::Matrix4f cameraRot)
{
	cy::Matrix4f cubeMVP = projMatrix * translationMatrix * cameraRot * cubeModel;
	cy::Matrix4f cubeMV = translationMatrix * cameraRot * cubeModel;

	cy::Matrix3f cubeNormal = cubeMV.GetSubMatrix3();
	cubeNormal.Invert();
	cubeNormal.Transpose();

	glassProg.Bind();
	glassProg["mvp"] = cubeMVP;
	glassProg["mv"] = cubeMV;
	glassProg["normalMatrix"] = cubeNormal;
	glassProg["model"] = cubeModel;
	glassProg["waterHeight"] = 25.0f;



	// Enable settings for blending and alpha values for transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);

	glBindVertexArray(cubeVao);
	glCullFace(GL_FRONT);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glCullFace(GL_BACK);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBindVertexArray(0);
}
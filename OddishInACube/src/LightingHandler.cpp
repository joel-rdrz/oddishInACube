#include "LightingHandler.h"
#include "ltc_matrix.hpp"

void LightingHandler::Initialize()
{
	prog.BuildFiles("Shaders/lighting.vert", "Shaders/lighting.frag");
	planeProg.BuildFiles("Shaders/plane.vert", "Shaders/plane.frag");

	// Setup LTC Inverse M Matrix
	ltc1Texture.Initialize();
	ltc1Texture.Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 64, 64, 0, GL_RGBA, GL_FLOAT, LTC1);
	ltc1Texture.SetFilteringMode(GL_LINEAR, GL_LINEAR);
	ltc1Texture.SetWrappingMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	
	// Setup LTC Texture for Fresnel
	ltc2Texture.Initialize();
	ltc2Texture.Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 64, 64, 0, GL_RGBA, GL_FLOAT, LTC2);
	ltc2Texture.SetFilteringMode(GL_LINEAR, GL_LINEAR);
	ltc2Texture.SetWrappingMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	areaLightVertices[0] = cy::Vec4f(20.0f, 10.0f, -5.0f, 1.0f); 
	areaLightVertices[1] = cy::Vec4f(20.0f, 10.0f, 5.0f, 1.0f); 
	areaLightVertices[2] = cy::Vec4f(20.0f, 20.0f, 5.0f, 1.0f);
	areaLightVertices[3] = cy::Vec4f(20.0f, 20.0f, -5.0f, 1.0f);
}

void LightingHandler::RenderLightingPass(
	cy::Matrix4f mvp, cy::Matrix3f normalMatrix, cy::Matrix4f mv, cy::Matrix4f matrixShadow,
	cy::Matrix4f translationMatrix, cy::Matrix4f cameraRot, int screenWidth, int screenHeight,
	GLuint vao, cy::TriMesh mesh, cy::Matrix4f projMatrix, cy::Matrix4f lightProjMatrix,
	cy::Matrix4f lightView, cy::Matrix4f T, cy::Matrix4f S, GLuint planeVao
)
{
	prog.Bind();
	glActiveTexture(GL_TEXTURE1);
	prog["shadow"] = 1;
	prog["mvp"] = mvp;
	prog["normalMatrix"] = normalMatrix;
	prog["mv"] = mv;
	prog["matrixShadow"] = matrixShadow;

	cy::Vec3f lightCameraCorners[4];
	for (int i = 0; i < 4; i++) {
		lightCameraCorners[i] = cy::Vec3f(translationMatrix * cameraRot * areaLightVertices[i]);
	}
	GLuint pointsLoc = glGetUniformLocation(prog.GetID(), "lightPoints");
	glUniform3fv(pointsLoc, 4, &lightCameraCorners[0].x);

	GLuint roughnessLoc = glGetUniformLocation(prog.GetID(), "roughness");
	glUniform1f(roughnessLoc, 0.3f); 


	cy::Vec4f lightPosWorld(64.0f, 42.0f, 64.0f, 1.0f);
	cy::Vec4f lightPosCamera = translationMatrix * cameraRot * lightPosWorld;
	prog["lightPos"] = cy::Vec3f(lightPosCamera);

	glActiveTexture(GL_TEXTURE0);
	tex.Bind(0);
	prog["tex"] = 0;

	ltc1Texture.Bind(2);
	prog["ltc1"] = 2;

	ltc2Texture.Bind(3);
	prog["ltc2"] = 3;


	glViewport(0, 0, screenWidth, screenHeight);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	prog.Bind();
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh.NF() * 3);

	cy::Matrix4f planeFull = cy::Matrix4f::Translation(cy::Vec3f(0.0f, -3.0f, 0.0f)) * cy::Matrix4f::RotationX(cy::Deg2Rad(-90.0f)) * cy::Matrix4f::Scale(75.0f);
	cy::Matrix4f planeMVP = projMatrix * translationMatrix * cameraRot * planeFull;
	cy::Matrix4f planeMV = translationMatrix * cameraRot * planeFull;
	cy::Matrix4f planeMLP = lightProjMatrix * lightView * planeFull;
	cy::Matrix3f planeNormalMatrix = planeMV.GetSubMatrix3();
	cy::Matrix4f matrixShadowPlane = T * S * planeMLP;

	planeNormalMatrix.Invert();
	planeNormalMatrix.Transpose();

	planeProg.Bind();
	planeProg["planeMVP"] = planeMVP;
	planeProg["mv"] = planeMV;
	planeProg["normalMatrix"] = planeNormalMatrix;
	planeProg["lightPos"] = cy::Vec3f(lightPosCamera);
	planeProg["matrixShadow"] = matrixShadowPlane;
	planeProg["shadow"] = 1;

	glBindVertexArray(planeVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

}
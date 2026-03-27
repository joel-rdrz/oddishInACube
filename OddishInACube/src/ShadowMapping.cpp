#include "ShadowMapping.h"

void ShadowMapping::Initialize()
{
	shadowMap.Initialize(
		true,
		shadowWidth,
		shadowHeight
	);
	shadowMap.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR);
	shadowProg.BuildFiles("Shaders/shadow.vert", "Shaders/shadow.frag");
}

void ShadowMapping::RenderShadowPass(cy::Matrix4f fullRotaion, GLuint vao, cy::TriMesh mesh) 
{
	lightView = cy::Matrix4f::View(cy::Vec3f(64.0, 42.0, 64.0), cy::Vec3f(0.0, 0.0, 0.0), cy::Vec3f(0.0, 1.0, 0.0));
	lightProjMatrix = cy::Matrix4f::Perspective(cy::Deg2Rad(30.0), float(shadowWidth) / float(shadowHeight),
	0.1f, 1000.0f);
	mlp = lightProjMatrix * lightView * fullRotaion;

	T = cy::Matrix4f::Translation(cy::Vec3f(0.5f, 0.5f, 0.5f - 0.00002));
	S = cy::Matrix4f::Scale(0.5);

	matrixShadow = T * S * mlp; // my little pony

	shadowMap.Bind();
	glViewport(0, 0, shadowWidth, shadowHeight);
	glClear(GL_DEPTH_BUFFER_BIT);

	shadowProg.Bind();
	shadowProg["mvp"] = mlp;

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh.NF() * 3);
	shadowMap.Unbind();

	shadowMap.BindTexture(1);
}
#include "ShadowMapping.h"

void ShadowMapping::Initialize()
{
	shadowMap.Initialize(true, 2, shadowWidth, shadowHeight, cy::GL::TYPE_FLOAT);
	shadowMap.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR);
	shadowMap.SetTextureWrappingMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	blurMap.Initialize(false, 2, shadowWidth, shadowHeight, cy::GL::TYPE_FLOAT);
	blurMap.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR);
	blurMap.SetTextureWrappingMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	shadowProg.BuildFiles("Shaders/shadow.vert", "Shaders/shadow.frag");
	blurProg.BuildFiles("Shaders/sat.vert", "Shaders/sat.frag");

	satPing.Initialize(false, 2, shadowWidth, shadowHeight, cy::GL::TYPE_FLOAT);
	satPing.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR);
	satPing.SetTextureWrappingMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	satPong.Initialize(false, 2, shadowWidth, shadowHeight, cy::GL::TYPE_FLOAT);
	satPong.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR);
	satPong.SetTextureWrappingMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	// Define a n quad for the post-processing SAT passes
	float quad[] = { -1,-1, 1,-1, 1,1, -1,-1, 1,1, -1,1 };
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void ShadowMapping::RenderShadowPass(cy::Matrix4f fullRotaion, GLuint vao, cy::TriMesh mesh)
{
	lightView = cy::Matrix4f::View(cy::Vec3f(64.0, 42.0, 64.0), cy::Vec3f(0.0, 0.0, 0.0), cy::Vec3f(0.0, 1.0, 0.0));
	lightProjMatrix = cy::Matrix4f::Perspective(cy::Deg2Rad(30.0), float(shadowWidth) / float(shadowHeight), 0.1f, 1000.0f);
	mlp = lightProjMatrix * lightView * fullRotaion;

	T = cy::Matrix4f::Translation(cy::Vec3f(0.5f, 0.5f, 0.5f - 0.00002));
	S = cy::Matrix4f::Scale(0.5);
	matrixShadow = T * S * mlp;

	// Output depth and depth^2 to the R and G channels.
	shadowMap.Bind();
	glViewport(0, 0, shadowWidth, shadowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 1);
	shadowProg.Bind();
	shadowProg["mvp"] = mlp;
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh.NF() * 3);
	glBindVertexArray(0);
	shadowMap.Unbind();

	// Build SAT using ping-pong with 10 horizontal passes then 10 vertical passes
	GLuint src = shadowMap.GetTextureID();
	bool toPong = true;

	// Horizontal passes
	for (int i = 0; i < 10; i++) {
		float stride = (float)(1 << i);
		cy::GLRenderTexture2D& dst = toPong ? satPong : satPing;
		dst.Bind();
		glViewport(0, 0, shadowWidth, shadowHeight);
		glClear(GL_COLOR_BUFFER_BIT);

		blurProg.Bind();
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, src);
		blurProg["vsm"] = 2;
		blurProg["blurDir"] = cy::Vec2f(stride / shadowWidth, 0.0f);
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		dst.Unbind();

		src = toPong ? satPong.GetTextureID() : satPing.GetTextureID();
		toPong = !toPong;
	}

	// Vertical passes
	for (int i = 0; i < 10; i++) {
		float stride = (float)(1 << i);
		cy::GLRenderTexture2D& dst = toPong ? satPong : satPing;
		dst.Bind();
		glViewport(0, 0, shadowWidth, shadowHeight);
		glClear(GL_COLOR_BUFFER_BIT);

		blurProg.Bind();
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, src);
		blurProg["vsm"] = 2;
		blurProg["blurDir"] = cy::Vec2f(0.0f, stride / shadowHeight);
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		dst.Unbind();

		src = toPong ? satPong.GetTextureID() : satPing.GetTextureID();
		toPong = !toPong;
	}

	// Bind final SAT to texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, src);
}
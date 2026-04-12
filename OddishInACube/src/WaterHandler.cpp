#include "WaterHandler.h"

void WaterHandler::Initialize(float waterHeight) {
	std::vector<cy::Vec3f> waterVertices;

	int gridSize = 128;
	float size = 100.0f; 
	float halfSize = size / 2.0f;

	// This is making the "infinite plane" of water by creating a grid of triangles.
	// The waterHeight is the y value of all the vertices, so it will be a flat plane.
	// THis will be replaced when we do what the paper actually suggests.
	for (int y = 0; y < gridSize - 1; y++) {
		for (int x = 0; x < gridSize - 1; x++) {

			float x0 = ((float)x / (gridSize - 1)) * 2.0f - 1.0f;
			float y0 = ((float)y / (gridSize - 1)) * 2.0f - 1.0f;
			float x1 = ((float)(x + 1) / (gridSize - 1)) * 2.0f - 1.0f;
			float y1 = ((float)(y + 1) / (gridSize - 1)) * 2.0f - 1.0f;

			// Notice the Z coordinate is 0.0f! 
			// This grid is flat against the camera lens, not in the 3D world.
			waterVertices.push_back(cy::Vec3f(x0, y0, 0.0f));
			waterVertices.push_back(cy::Vec3f(x0, y1, 0.0f));
			waterVertices.push_back(cy::Vec3f(x1, y0, 0.0f));

			waterVertices.push_back(cy::Vec3f(x1, y0, 0.0f));
			waterVertices.push_back(cy::Vec3f(x0, y1, 0.0f));
			waterVertices.push_back(cy::Vec3f(x1, y1, 0.0f));
		}
	}

	vertexCount = waterVertices.size();

	// Water intialization stuff
	glGenVertexArrays(1, &waterVao);
	glBindVertexArray(waterVao);
	glGenBuffers(1, &waterVbo);
	glBindBuffer(GL_ARRAY_BUFFER, waterVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * waterVertices.size(), waterVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(cy::Vec3f), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	waterProg.BuildFiles("Shaders/water.vert", "Shaders/water.frag");
	maskProg.BuildFiles("Shaders/mask.vert", "Shaders/mask.frag");

}

void WaterHandler::RenderWater(cy::Matrix4f projMatrix, cy::Matrix4f viewMatrix, float worldWaterHeight, float time, cy::Vec3f lightDir, cy::Vec3f cameraPos, float screenWidth, float screenHeight) {
	
	reflectionFBO.BindTexture(1);
	refractionFBO.BindTexture(2);

	cy::Matrix4f viewProjMatrix = projMatrix * viewMatrix;

	cy::Matrix4f projector = viewProjMatrix;
	projector.Invert();

	waterProg.Bind();
	waterProg["mvp"] = viewProjMatrix;
	waterProg["projector"] = projector;
	waterProg["waterHeight"] = worldWaterHeight;
	waterProg["time"] = time;
	waterProg["lightDir"] = lightDir;
	waterProg["cameraPos"] = cameraPos;
	waterProg["screenWidth"] = screenWidth;
	waterProg["screenHeight"] = screenHeight;
	waterProg["reflectionTex"] = 1;
	waterProg["refractionTex"] = 2;

	glActiveTexture(GL_TEXTURE0);
	noiseTex.Bind(0);
	waterProg["noiseTex"] = 0;
	waterProg["scale"] = 0.3f;
	glBindVertexArray(waterVao);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void WaterHandler::RenderMask(cy::Matrix4f mvp, GLuint maskVao) {
	maskProg.Bind();
	maskProg["mvp"] = mvp;

	glBindVertexArray(maskVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void WaterHandler::initFrameBuffers(int width, int height) {
	reflectionFBO.Initialize(true, 3, width, height);
	refractionFBO.Initialize(true, 3, width, height);
}
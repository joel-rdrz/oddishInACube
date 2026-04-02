#include "WaterHandler.h"

void WaterHandler::Initialize(float waterHeight) {
	std::vector<cy::Vec3f> waterVertices;

	int gridSize = 64;
	float size = 100.0f; 
	float halfSize = size / 2.0f;

	// This is making the "infinite plane" of water by creating a grid of triangles.
	// The waterHeight is the y value of all the vertices, so it will be a flat plane.
	// THis will be replaced when we do what the paper actually suggests.
	for (int z = 0; z < gridSize - 1; z++) {
		for (int x = 0; x < gridSize - 1; x++) {
			float x0 = ((float)x / (gridSize - 1)) * size - halfSize;
			float z0 = ((float)z / (gridSize - 1)) * size - halfSize;
			float x1 = ((float)(x + 1) / (gridSize - 1)) * size - halfSize;
			float z1 = ((float)(z + 1) / (gridSize - 1)) * size - halfSize;

			waterVertices.push_back(cy::Vec3f(x0, waterHeight, z0));
			waterVertices.push_back(cy::Vec3f(x0, waterHeight, z1));
			waterVertices.push_back(cy::Vec3f(x1, waterHeight, z0));
			waterVertices.push_back(cy::Vec3f(x1, waterHeight, z0));
			waterVertices.push_back(cy::Vec3f(x0, waterHeight, z1));
			waterVertices.push_back(cy::Vec3f(x1, waterHeight, z1));
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
}

void WaterHandler::RenderWater(cy::Matrix4f mvp) {
	waterProg.Bind();
	waterProg["mvp"] = mvp;

	glBindVertexArray(waterVao);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}
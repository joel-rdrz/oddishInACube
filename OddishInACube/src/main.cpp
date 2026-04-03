#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cyTriMesh.h>
#include <cyMatrix.h>
#include <cyGL.h>
#include <ShadowMapping.h>
#include <LightingHandler.h>
#include <GlassCube.h>
#include <WaterHandler.h>
#include "lodepng.h"

void myDisplay();
void myKeyboard(unsigned char key, int x, int y);
void myMouse(int button, int state, int x, int y);
void myMouseMotion(int x, int y);
void myReshape(int x, int y);


cy::TriMesh mesh;
std::vector<cy::Vec3f> vertexBufferData;
std::vector<cy::Vec3f> normalBufferData;
std::vector<cy::Vec3f> textureBufferData;
GLuint vao;
float xRot = -89.5;
float yRot = 10;
float distance = 100.0;

int currMouseClick = -1;
int mouseXPos = 0;
int mouseYPos = 0;
unsigned int width;
unsigned int height;
std::vector<unsigned char> imageData;
cyGLTexture2D tex;
char* textureFile;


int screenWidth = 800;
int screenHeight = 600;

float cameraXRot = 0.24;
float cameraYRot = 0.0;
float planeDistance = 5.0;
float waterHeight = 25.0f;
GLuint planeVao;
GLuint planeVbo;
GLuint texBuffer;

boolean altPressed = false;
GLuint maskVao;
GLuint  maskVbo;

ShadowMapping shadowObj;
LightingHandler lightObj;
WaterHandler waterObj;

GlassCube glassCube;

cy::GLRenderTexture2D renderBuffer;

int main(int argc, char** argv)
{
	//Initializations
	glutInit(&argc, argv);

	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Shadow Mapping 5610 Project 7");
	glutDisplayFunc(myDisplay);
	glutKeyboardFunc(myKeyboard);
	glutReshapeFunc(myReshape);
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenVertexArrays(1, &planeVao);
	glBindVertexArray(planeVao);

	GLuint buffer;
	GLuint normalBufferGl;

	glGenBuffers(1, &buffer);
	glGenBuffers(1, &normalBufferGl);
	glGenBuffers(1, &planeVbo);


	if (argc < 2)
	{
		std::cout << "Please provide an obj file for parsing.";
		return -1;
	}

	char* obj = argv[1];
	//obj = "Assets/" + obj;
	bool success = mesh.LoadFromFileObj(obj);

	//Building all the datas to put into our buffers, I know this is inefficient.
	for (unsigned int i = 0; i < mesh.NF(); i++) {
		cy::TriMesh::TriFace face = mesh.F(i);

		vertexBufferData.push_back(mesh.V(face.v[0]));
		vertexBufferData.push_back(mesh.V(face.v[1]));
		vertexBufferData.push_back(mesh.V(face.v[2]));
	}

	for (unsigned int i = 0; i < mesh.NF(); i++) {
		cy::TriMesh::TriFace face = mesh.FN(i);

		normalBufferData.push_back(mesh.VN(face.v[0]));
		normalBufferData.push_back(mesh.VN(face.v[1]));
		normalBufferData.push_back(mesh.VN(face.v[2]));
	}

	// Loop for texture coordinates
	for (unsigned int i = 0; i < mesh.NF(); i++) {
		cy::TriMesh::TriFace textureFace = mesh.FT(i);

		textureBufferData.push_back(mesh.VT(textureFace.v[0]));
		textureBufferData.push_back(mesh.VT(textureFace.v[1]));		
		textureBufferData.push_back(mesh.VT(textureFace.v[2]));
	}

	shadowObj.Initialize();

	//Just da coordinated for the plane
	float squarePlane[] = {
		-1.0, -1.0, 0.0,  0.0, 0.0, 1.0,
		1.0, -1.0, 0.0,  0.0, 0.0, 1.0,
		1.0, 1.0, 0.0,  0.0, 0.0, 1.0,

		-1.0, -1.0, 0.0,  0.0, 0.0,1.0,
		1.0, 1.0, 0.0,    0.0, 0.0,1.0,
		-1.0, 1.0, 0.0,   0.0, 0.0, 1.0
	};
	//Actually binds our buffers and all that.
	glBindBuffer(GL_ARRAY_BUFFER, planeVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squarePlane), squarePlane, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * vertexBufferData.size(), vertexBufferData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(cy::Vec3f), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, normalBufferGl);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * normalBufferData.size(), normalBufferData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(cy::Vec3f), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	// THis is basically the outline of the cube. THink of it like a cookie cutter that is the shape of the cube,
	// we will use this to stencil the water so that it only appears inside the cube, this is based on section 3.5.3 of the Water Rendering paper.
	float maskPlane[] = {
		-23.0f, waterHeight, -23.0f,
		 23.0f, waterHeight, -23.0f,
		 23.0f, waterHeight,  23.0f,

		-23.0f, waterHeight, -23.0f,
		 23.0f, waterHeight,  23.0f,
		-23.0f, waterHeight,  23.0f
	};

	glGenVertexArrays(1, &maskVao);
	glBindVertexArray(maskVao);
	glGenBuffers(1, &maskVbo);
	glBindBuffer(GL_ARRAY_BUFFER, maskVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(maskPlane), maskPlane, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	lightObj.Initialize();
	glassCube.Initialize();
	waterObj.Initialize(waterHeight);

	if (mesh.NM() > 0) {
		textureFile = mesh.M(0).map_Kd.data; // Gets the diffuse texture
	}

	if (textureFile) {
		unsigned error = lodepng::decode(imageData, width, height, textureFile);
		if (!error) {
			lightObj.tex.Initialize();
			lightObj.tex.SetImage(imageData.data(), 4, width, height);
			lightObj.tex.BuildMipmaps();
		}
		else {
			std::cout << "Error loading texture" << std::endl;
		}
	}

	glBindVertexArray(vao);
	glGenBuffers(1, &texBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * textureBufferData.size(), textureBufferData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT,GL_FALSE, sizeof(cy::Vec3f), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	glClearColor(0, 0, 0, 1.0);
	glViewport(0, 0, screenWidth, screenHeight);

	glutMouseFunc(myMouse);
	glutMotionFunc(myMouseMotion);

	glutMainLoop();
	return 0;
}
void myKeyboard(unsigned char key, int x, int y)
{
	if (key == 27)
	{
		glutLeaveMainLoop();
	}

	if (key == 'l' || key == 'L') {
		distance = 99.86f;
		cameraYRot = cy::Deg2Rad(-45.0f);
		cameraXRot = cy::Deg2Rad(24.9f);
		glutPostRedisplay();
	}
}
void myDisplay()
{
	cy::Matrix3f yRotMatrix = cy::Matrix3f::RotationY(yRot);
	cy::Matrix3f xRotMatrix = cy::Matrix3f::RotationX(xRot);

	cy::Matrix4f angleRotation = cy::Matrix4f::RotationX(cy::Deg2Rad(80.0f)) * cy::Matrix4f::RotationY(cy::Deg2Rad(180.0f));

	cy::Matrix4f fullRotaion = cy::Matrix4f::Scale(20.0f) * cy::Matrix4f(yRotMatrix * xRotMatrix) * angleRotation;
	cy::Matrix4f cameraRot = cy::Matrix4f::RotationY(cameraYRot) * cy::Matrix4f::RotationX(cameraXRot);
	cy::Matrix4f translationMatrix = cy::Matrix4f::Translation(cy::Vec3f(0.0, 0.0, -distance));
	cy::Matrix4f projMatrix = cy::Matrix4f::Perspective(cy::Deg2Rad(40.0), float(screenWidth) / float(screenHeight),
		0.1f, 1000.0f);

	cy::Matrix4f mv = translationMatrix * cameraRot * fullRotaion;
	cy::Matrix4f mvp = projMatrix * translationMatrix * cameraRot * fullRotaion;

	cy::Matrix3f normalMatrix = mv.GetSubMatrix3();
	normalMatrix.Invert();
	normalMatrix.Transpose();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glassCube.SetModel();

	shadowObj.RenderShadowPass(fullRotaion, vao, mesh);

	lightObj.RenderLightingPass(mvp, normalMatrix, mv, shadowObj.matrixShadow, 
		translationMatrix, cameraRot, screenWidth, screenHeight,
		vao, mesh, projMatrix, shadowObj.lightProjMatrix,
		shadowObj.lightView, shadowObj.T, shadowObj.S, planeVao);

	// What this next part here does is it uses the stencil buffer to only draw the water where the water should be,
	// this is done by first drawing a plane where the water should be and setting the stencil to 1 there, then we only draw the water where the stencil is 1,
	// this allows us to have the water inside the glass cube without it rendering on top of the cube itself, this is based on section 3.5.3 of the Water Rendering paper.
	// We are effecitvely taking our infinite water plane and forcing it to live IN our cube this is like the volcano example in the paper. I'm adding this note becuaset this
	// was a bit confusing to implement and I want to make sure the logic is clear in the future when I look back at this code.

	glEnable(GL_STENCIL_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// Draw the "Outline of the Lake" Section 3.5.3 of Water Rendering paper on canvas, this will set the stencil to 1 where the water should be
	waterObj.waterProg.Bind();
	cy::Matrix4f maskMVP = projMatrix * translationMatrix * cameraRot;
	waterObj.RenderMask(maskMVP, maskVao);
	glBindVertexArray(maskVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	
	glDisable(GL_CULL_FACE);

	cy::Matrix4f viewMatrix = translationMatrix * cameraRot;
	waterObj.RenderWater(projMatrix,viewMatrix, waterHeight);

	glEnable(GL_CULL_FACE);
	glDisable(GL_STENCIL_TEST);

	// Draw the glass cube at the end so it doesn't block the water inside it
	glassCube.Render(projMatrix, translationMatrix, cameraRot);

	glutSwapBuffers();
}

void myMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		currMouseClick = button;
	}
	else
	{
		currMouseClick = -1;
	}
	mouseXPos = x;
	mouseYPos = y;
}
void myMouseMotion(int x, int y)
{
	int mods = glutGetModifiers();
	altPressed = mods & GLUT_ACTIVE_ALT;
	if (!altPressed)
	{
		if (currMouseClick == GLUT_LEFT_BUTTON)
		{
			cameraYRot += (x - mouseXPos) * 0.005;
			cameraXRot += (y - mouseYPos) * 0.005;
		}
		else if (currMouseClick == GLUT_RIGHT_BUTTON)
		{
			distance += y - mouseYPos;
		}
	}
	else
	{
		if (currMouseClick == GLUT_LEFT_BUTTON)
		{
			yRot += (x - mouseXPos) * 0.05;
			xRot += (y - mouseYPos) * 0.05;
		}
		else if (currMouseClick == GLUT_RIGHT_BUTTON)
		{
			distance += y - mouseYPos;
		}
	}

	mouseXPos = x;
	mouseYPos = y;

	glutPostRedisplay();
}

void myReshape(int x, int y)
{
	screenWidth = x;
	screenHeight = y;
	glViewport(0, 0, screenWidth, screenHeight);
	glutPostRedisplay();
}


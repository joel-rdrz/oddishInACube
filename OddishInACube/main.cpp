#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cyTriMesh.h>
#include <cyMatrix.h>
#include <cyGL.h>


void myDisplay();
void myKeyboard(unsigned char key, int x, int y);
void myMouse(int button, int state, int x, int y);
void myMouseMotion(int x, int y);
void myReshape(int x, int y);


cy::TriMesh mesh;
std::vector<cy::Vec3f> vertexBufferData;
std::vector<cy::Vec3f> normalBufferData;
GLuint vao;
cy::GLSLProgram prog;
cy::GLSLProgram shadowProg;
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

int screenWidth = 800;
int screenHeight = 600;

int shadowWidth = 1024;
int shadowHeight = 1024;
float cameraXRot = 0.24;
float cameraYRot = 0.0;
float planeDistance = 5.0;
cy::GLSLProgram planeProg;
GLuint planeVao;
GLuint planeVbo;

boolean altPressed = false;


cy::GLRenderTexture2D renderBuffer;
cy::GLRenderDepth2D shadowMap;

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

	//Set up the shadowmap as shown in class
	shadowMap.Initialize(
		true,
		shadowWidth,
		shadowHeight
	);
	shadowMap.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR);


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

	prog.BuildFiles("shader.vert", "shader.frag");
	planeProg.BuildFiles("planeShader.vert", "planeShader.frag");
	shadowProg.BuildFiles("shadow.vert", "shadow.frag");

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

	cy::Matrix4f fullRotaion = cy::Matrix4f(yRotMatrix * xRotMatrix);
	cy::Matrix4f cameraRot = cy::Matrix4f::RotationY(cameraYRot) * cy::Matrix4f::RotationX(cameraXRot);
	cy::Matrix4f translationMatrix = cy::Matrix4f::Translation(cy::Vec3f(0.0, 0.0, -distance));
	cy::Matrix4f projMatrix = cy::Matrix4f::Perspective(cy::Deg2Rad(40.0), float(screenWidth) / float(screenHeight),
		0.1f, 1000.0f);

	cy::Matrix4f mv = translationMatrix * cameraRot * fullRotaion;
	cy::Matrix4f mvp = projMatrix * translationMatrix * cameraRot * fullRotaion;

	cy::Matrix3f normalMatrix = mv.GetSubMatrix3();
	normalMatrix.Invert();
	normalMatrix.Transpose();

	prog.Bind();

	//Lighting matrices

	cy::Matrix4f lightView = cy::Matrix4f::View(cy::Vec3f(64.0, 42.0, 64.0), cy::Vec3f(0.0, 0.0, 0.0), cy::Vec3f(0.0, 1.0, 0.0));
	cy::Matrix4f lightProjMatrix = cy::Matrix4f::Perspective(cy::Deg2Rad(30.0), float(shadowWidth) / float(shadowHeight),
		0.1f, 1000.0f);
	cy::Matrix4f mlp = lightProjMatrix * lightView * fullRotaion;

	cy::Matrix4f T = cy::Matrix4f::Translation(cy::Vec3f(0.5f, 0.5f, 0.5f - 0.00002));
	cy::Matrix4f S = cy::Matrix4f::Scale(0.5);

	cy::Matrix4f matrixShadow = T * S * mlp; // my little pony

	shadowMap.Bind();
	glViewport(0, 0, shadowWidth, shadowHeight);
	glClear(GL_DEPTH_BUFFER_BIT);

	shadowProg.Bind();
	shadowProg["mvp"] = mlp;

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh.NF() * 3);
	shadowMap.Unbind();

	shadowMap.BindTexture(1);
	prog.Bind();
	prog["shadow"] = 1;
	prog["mvp"] = mvp;
	prog["normalMatrix"] = normalMatrix;
	prog["mv"] = mv;
	prog["matrixShadow"] = matrixShadow;
	cy::Vec4f lightPosWorld(64.0f, 42.0f, 64.0f, 1.0f);
	cy::Vec4f lightPosCamera = translationMatrix * cameraRot * lightPosWorld; 
	prog["lightPos"] = cy::Vec3f(lightPosCamera); 


	glViewport(0, 0, screenWidth, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	prog.Bind();
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh.NF() * 3);

	cy::Matrix4f planeFull = cy::Matrix4f::Translation(cy::Vec3f(0.0f, -3.0f, 0.0f)) * cy::Matrix4f::RotationX(cy::Deg2Rad(-90.0f)) * cy::Matrix4f::Scale(75.0f);
	cy::Matrix4f planeMVP = projMatrix * translationMatrix * cameraRot * planeFull;
	cy::Matrix4f planeMV =  translationMatrix * cameraRot * planeFull;
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


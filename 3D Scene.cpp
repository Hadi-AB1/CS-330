/*
Name: Hadi Abdelaal.
Class: CS-330.
Instructor: Scott Gray.
Date: 11/16/2023.
Assignment: Beginning a 3D Scene.
*/

#include <iostream>
#include <random>
#include <vector>
#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtx/transform.hpp>
#include <gtc/type_ptr.hpp>
using namespace std;
namespace
{ //This is the Variable for our window title.
	const char* const WINDOW_TITLE = "Beginning a 3D Scene: (Hadi Abdelaal/CS-330)";
	//Window width and height variables. 
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 800;
	struct GLMesh {
		GLuint vao; 
		GLuint vbos[2];
		GLuint nIndices;
	};
	//Tha main GLFW Window.
	GLFWwindow* uWindow = nullptr;
	GLMesh pencilMesh;
	GLuint uProgramId;
}
// Initializing the program also setting the window size, and rendering the graphic on the screen.
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UCreateMesh(GLMesh& mesh, vector<GLfloat>& verts, vector<GLushort>& indices);
void UDestroyMesh(GLMesh& mesh);
bool UCreateShaderProgram(const char* vtxShaderSource, const char*
	fragShaderSource, GLuint&
	programId);
void UDestroyShaderProgram(GLuint programId);
void URender();
const char* vertexShaderSource = "#version 440 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec4 colorFromVBO;\n"
"out vec4 colorFromVS;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
" gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
" colorFromVS = colorFromVBO;\n"
"}\0";
// The source code of the fragment shader.
const char* fragmentShaderSource = "#version 440 core\n"
"in vec4 colorFromVS;\n"
"out vec4 FragColor;\n"
"void main()\n""{\n"
" FragColor = colorFromVS;\n""}\n\0";
//The vectors to hold the shape vertices and indices. 
vector<GLfloat> pencilVerts = {
    // Colors (r,g,b,a) // Pyramid vertices
	 0.20f, -0.65f, -0.5f, 0.70f, 0.50f, 0.40f, 0.0f, 
	 0.20f, -0.75f, 0.2f, 0.60f, 0.50f, 0.50f, 0.0f, 
	 - 0.40f, -0.95f, -0.4f, 0.40f, 0.50f, 0.30f, 0.0f, 
	 - 0.20f, -0.75f, 0.2f, 0.50f, 0.50f, 0.30f, 0.0f, 
	 0.0f, -0.3f, -0.4f, 0.80f, 0.50f, 0.30f, 0.0f, 
	// Cube vertices // Colors (r,g,b,a)
	 0.50f, -3.5f, -0.4f, 0.17f, 0.40f, 0.70f, 0.0f, 
	 0.50f, -3.5f, 0.1f, 0.17f, 0.40f, 0.70f, 0.0f,  
	 - 0.50f, -3.5f, -0.4f, 0.17f, 0.40f, 0.70f, 0.0f, 
	 - 0.50f, -3.5f, 0.2f, 0.17f, 0.40f, 0.70f, 0.0f,
};
vector<GLushort> pencilIndices = {
	0,1,2,
	2,3,1,
	1,4,3,
	0,1,4,
	0,2,4,
	2,3,4,
	3,1,6,
	5,6,1,
	1,0,5,
	7,5,0,
	0,2,7,
	8,7,2,
	2,3,8,
	8,6,5,
    5,7,8,
    8,6,3,
};
// Main FUnction. 
int main(int argc, char* argv[])
{
	if (!UInitialize(argc, argv, &uWindow))
		return EXIT_FAILURE;
	// Exitting the program if  the creation of the shader program fails.
	if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource,
		uProgramId))
		return EXIT_FAILURE;
	UCreateMesh(pencilMesh, pencilVerts, pencilIndices);
	//This will render the loop. 
	while (!glfwWindowShouldClose(uWindow))
	{
		UProcessInput(uWindow);
		URender();
		glfwPollEvents();
	}
	//This will release the mesh data.
	UDestroyMesh(pencilMesh);
	UDestroyShaderProgram(uProgramId);
	exit(EXIT_SUCCESS);
}
// This will initialize the GLEW, GLFW.
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
	// Initialzing and configuration of GLFW.
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	//Window creation for GLFW. 
	* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL,
		NULL);
	if (*window == NULL) 
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, UResizeWindow);
	// GLEW: initialize.
	glewExperimental = GL_TRUE;
	GLenum GlewInitResult = glewInit();
	if (GLEW_OK != GlewInitResult)
	{
		cerr << glewGetErrorString(GlewInitResult) << endl;
		return false;
	}
	cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;
	return true;
}
//This will process all the inputs.
void UProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
void UResizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
//This will render the graphic in our window.
void URender() {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// This will create a pencil. 
	glm::mat4 pencilScale = glm::scale(glm::vec3(1.5f, 2.0f, 1.0f));
	glm::mat4 pencilRotation = glm::rotate(16.05f, glm::vec3(0.52f, 0.50f,
		0.0f));
	glm::mat4 pencilTranslation = glm::translate(glm::vec3(3.0f, 0.0f, -4.0f));
	glm::mat4 pencilModel = pencilTranslation * pencilRotation * pencilScale;
	//Creating a world and clip space matrices.
	glm::mat4 view = glm::translate(glm::vec3(0.0f, 0.0f, -5.0f));
	glm::mat4 projection = glm::perspective(65.0f, (GLfloat)WINDOW_WIDTH /
		(GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
	GLuint modelLoc = glGetUniformLocation(uProgramId, "model");
	GLuint viewLoc = glGetUniformLocation(uProgramId, "view");
	GLuint projLoc = glGetUniformLocation(uProgramId, "projection");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	// Drawing the penccil.
	glBindVertexArray(pencilMesh.vao);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pencilModel));
	glDrawElements(GL_TRIANGLES, pencilIndices.size(), GL_UNSIGNED_SHORT, NULL);
	glBindVertexArray(0);
	glfwSwapBuffers(uWindow);
}
void UCreateMesh(GLMesh& mesh, vector<GLfloat>& verts, vector<GLushort>& indices) {
	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);
	//Generating two buffers.
	glGenBuffers(2, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verts.size(), &verts[0],
		GL_STATIC_DRAW);
	//This will create buffer object for the index.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort) * indices.size(),
		&indices[0],
		GL_STATIC_DRAW);
	const GLuint floatsPerVertex = 3; // The number of coordinates per vertex.
	const GLuint floatsPerColor = 4; 
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)
			(sizeof(float) *
				floatsPerVertex));
	glEnableVertexAttribArray(1); // This will specifies the intial position of the colors in the buffer.
}
void UDestroyMesh(GLMesh& mesh) {
	glDeleteVertexArrays(1, &mesh.vao);
	glDeleteBuffers(2, mesh.vbos);
}
bool UCreateShaderProgram(const char* vtxShaderSource, const char*
	fragShaderSource, GLuint&
	programId) {
	int success = 0;
	char infoLog[512];
	programId = glCreateProgram();
	// Creating  the vertex and fragment shader objects.
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
	glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);
	glCompileShader(vertexShaderId);
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShaderId, sizeof(infoLog), NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog <<
			endl;
		return false;
	}
	glCompileShader(fragmentShaderId);
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog <<
			endl;
		return false;
	}
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
		return false;
	}
	glUseProgram(programId);
	return true;
}
void UDestroyShaderProgram(GLuint programId) {
	glDeleteProgram(programId);
}
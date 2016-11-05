#include <string>
#include <iostream>

// GLEW
#define GLEW_STATIC
#include <gl/glew.h>
#include <GLFW\glfw3.h>
#include <gl/glut.h>
#include <gl/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/camera.h"
#include "../include/shader.h"

#include <SOIL.h>
#include "HalfEdge.h"

GLuint screenWidth = 800, screenHeight = 600;

// Camera
Camera camera(glm::vec3(0.5f, 2.0f, 0.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
bool keys[1024];

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
int drawingMode = 0;

void drawPointMode(Shader shader, HalfEdgeModel model);
void drawWireframeMode(Shader shader, HalfEdgeModel model);
void drawPlainMode(Shader shader, HalfEdgeModel model);
void drawPlainWireframeMode(Shader shader, HalfEdgeModel model);

void(*drawModel[4])(Shader shader, HalfEdgeModel model) = { drawPointMode, drawWireframeMode, drawPlainMode, drawPlainWireframeMode };

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_actions();

int main(){

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "AssignmentB4", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);

	HalfEdgeModel mymodel("eight.uniform.obj");
	Shader shader("vertexShader.glsl", "fragmentShader.glsl");
	
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();
		do_actions();

		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		(*drawModel[drawingMode])(shader, mymodel);

		glfwSwapBuffers(window);

	}


	glfwTerminate();
	return 0;
}

void drawPointMode(Shader shader, HalfEdgeModel model){
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	shader.Use();
	glUniform3f(glGetUniformLocation(shader.Program, "shadeColor"), 1.0f, 0.0f, 0.0f);
	shader.SetCameraProperty(screenWidth, screenHeight, 0.1f, 1000.0f, camera);
	model.Draw(shader);
}
void drawWireframeMode(Shader shader, HalfEdgeModel model){
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	shader.Use();
	glUniform3f(glGetUniformLocation(shader.Program, "shadeColor"), 1.0f, 0.0f, 0.0f);
	shader.SetCameraProperty(screenWidth, screenHeight, 0.1f, 1000.0f, camera);
	model.Draw(shader);
}
void drawPlainMode(Shader shader, HalfEdgeModel model){
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	shader.Use();
	glUniform3f(glGetUniformLocation(shader.Program, "shadeColor"), 1.0f, 0.0f, 0.0f);
	shader.SetCameraProperty(screenWidth, screenHeight, 0.1f, 1000.0f, camera);
	model.Draw(shader);
}
void drawPlainWireframeMode(Shader shader, HalfEdgeModel model){
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	shader.Use();

	glUniform3f(glGetUniformLocation(shader.Program, "shadeColor"), 0.0f, 0.0f, 0.0f);
	shader.SetCameraProperty(screenWidth, screenHeight, 0.1f, 1000.0f, camera);
	model.Draw(shader);

	glUniform3f(glGetUniformLocation(shader.Program, "shadeColor"), 1.0f, 0.0f, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	model.Draw(shader);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}

void do_actions(){
	if (keys[GLFW_KEY_1])
		drawingMode = 0;
	if (keys[GLFW_KEY_2])
		drawingMode = 1;
	if (keys[GLFW_KEY_3])
		drawingMode = 2;
	if (keys[GLFW_KEY_4])
		drawingMode = 3;
}
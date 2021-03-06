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
#include "../include/HalfEdge.h"

GLuint screenWidth = 800, screenHeight = 600;

// Camera
Camera camera(glm::vec3(0.5f, 2.0f, 0.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
float cursorPosX, cursorPosY;
float prevPosX, prevPosY;
float lastHitX, lastHitY;
bool firstMouse = true;
bool inViewRotate = false;
bool inViewPan = false;
bool inModelRotate = false;
int lastModelRotateDirection = 0;
float lastModelRotationAngle = 0;
int lastModelRotationRow = 0;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mousebutton_callback(GLFWwindow* window, int button, int action, int mode);
void do_actions();

void drawBlue(Shader shader);
void drawRed(Shader shader);
void drawGreen(Shader shader);
void drawYellow(Shader shader);
int currentColor = 0;

void(*drawColor[4])(Shader shader) = { drawBlue, drawRed, drawGreen, drawYellow };

int main(){

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "AssignmentB5", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mousebutton_callback);

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

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		shader.Use();
		(*drawColor[currentColor])(shader);
		glUniform3f(glGetUniformLocation(shader.Program, "AmbientColor"), 0.2f, 0.2f, 0.2f);
		shader.SetCameraProperty(screenWidth, screenHeight, 0.1f, 1000.0f, camera);
		shader.SetDefaultLight();
		shader.SetSpotLight(0, camera.Position, camera.Front, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f, 40, 60);
		mymodel.Draw(shader);

		glfwSwapBuffers(window);

	}


	glfwTerminate();
	return 0;
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
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (inViewRotate){
		float xoffset = -cursorPosX + prevPosX;
		float yoffset = -cursorPosY + prevPosY;
		camera.ProcessMouseRotate(xoffset, yoffset);
	}
	if (inViewPan){
		float xoffset = -cursorPosX + prevPosX;
		float yoffset = cursorPosY - prevPosY;
		camera.ProcessMousePan(xoffset, yoffset);
	}

	if (keys[GLFW_KEY_1])
		currentColor = 0;
	if (keys[GLFW_KEY_2])
		currentColor = 1;
	if (keys[GLFW_KEY_3])
		currentColor = 2;
	if (keys[GLFW_KEY_4])
		currentColor = 3;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	prevPosX = cursorPosX;
	prevPosY = cursorPosY;
	cursorPosX = xpos;
	cursorPosY = ypos;
}


void mousebutton_callback(GLFWwindow* window, int button, int action, int mode){
	if (button == GLFW_MOUSE_BUTTON_LEFT && keys[GLFW_KEY_LEFT_ALT] == true){
		if (action == GLFW_RELEASE){
			inViewRotate = false;
		}
		else{
			inViewRotate = true;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && keys[GLFW_KEY_LEFT_ALT] == true){
		if (action == GLFW_RELEASE){
			inViewPan = false;
		}
		else{
			inViewPan = true;
		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void drawBlue(Shader shader){
	glUniform3f(glGetUniformLocation(shader.Program, "Albeido"), 0.0f, 0.0f, 1.0f);
}
void drawRed(Shader shader){
	glUniform3f(glGetUniformLocation(shader.Program, "Albeido"), 1.0f, 0.0f, 0.0f);
}
void drawGreen(Shader shader){
	glUniform3f(glGetUniformLocation(shader.Program, "Albeido"), 0.0f, 1.0f, 0.0f);
}
void drawYellow(Shader shader){
	glUniform3f(glGetUniformLocation(shader.Program, "Albeido"), 1.0f, 1.0f, 0.0f);
}
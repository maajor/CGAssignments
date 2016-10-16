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

#include "terrain.h"

GLuint screenWidth = 800, screenHeight = 600;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
bool keys[1024];

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

void move_action();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mousebutton_callback(GLFWwindow* window, int button, int action, int mode);

GLfloat lastX = 400, lastY = 300;
float cursorPosX, cursorPosY;
float prevPosX, prevPosY;
float lastHitX, lastHitY;
bool firstMouse = true;
bool inViewRotate = false;
bool inViewPan = false;

int main(){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "AssignmentA6", nullptr, nullptr); // Windowed
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
	
	Shader shader("shader/terrainVertexShader.glsl", "shader/terrainFragmentShader.glsl");

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	Terrain myterrain(shader);
	myterrain.loadHeightmap("textures/heightmap.bmp");
	myterrain.loadTexture("textures/terrain-texture3.bmp", NULL, NULL, NULL);
	

	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();
		move_action();

		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myterrain.terrainShader.Use();  

		myterrain.terrainShader.SetDefaultLight();
		myterrain.terrainShader.SetCameraProperty(screenWidth, screenHeight, 0.1f, 1000.0f, camera);
		
		myterrain.render();

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

void move_action(){
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset / 10);
}